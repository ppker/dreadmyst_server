// Dreadmyst Server - Main Entry Point
// Task 2.8: Basic Game Loop
// Task 2.9: Tick-Based Update System
// Task 2.11: Server Shutdown Graceful

#include "stdafx.h"
#include "Core/Config.h"
#include "Core/Logger.h"
#include "Core/GameClock.h"
#include "Database/AsyncSaver.h"
#include "Database/DatabaseManager.h"
#include "Database/GameData.h"
#include "Network/Session.h"
#include "Network/SessionManager.h"
#include "Network/PacketRouter.h"
#include "World/WorldManager.h"
#include "World/MapManager.h"
#include "Systems/VendorSystem.h"
#include "Systems/GossipSystem.h"
#include "Systems/GuildSystem.h"
#include "SfSocket.h"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <csignal>
#include <atomic>

// Global flag for graceful shutdown
static std::atomic<bool> g_running{true};

// Signal handler for graceful shutdown (Ctrl+C)
void signalHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        LOG_INFO("Shutdown signal received...");
        g_running = false;
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Register signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Print startup banner
    LOG_INFO("===========================================");
    LOG_INFO("  Dreadmyst Server v0.1.0");
    LOG_INFO("===========================================");

    // Load configuration
    const char* configPath = "data/server.ini";
    if (!sConfig.load(configPath)) {
        LOG_WARN("Could not load %s, using defaults", configPath);
    }

    LOG_INFO("Server Port: %d", sConfig.getServerPort());
    LOG_INFO("Max Connections: %d", sConfig.getMaxConnections());

    // Initialize databases
    if (!sDatabase.open(sConfig.getServerDbPath())) {
        LOG_WARN("Could not open server database, creating new one");
        if (!sDatabase.open(sConfig.getServerDbPath())) {
            LOG_ERROR("Failed to create server database");
            return 1;
        }
    }

    // Execute schema (creates tables if they don't exist)
    if (!sDatabase.executeFile("data/schema.sql")) {
        LOG_WARN("Could not execute schema.sql (may already exist)");
    }

    // Load game data
    if (!sGameData.loadFromDatabase(sConfig.getGameDbPath())) {
        LOG_ERROR("Failed to load game data from %s", sConfig.getGameDbPath().c_str());
        return 1;
    }

    // Load vendor data (Phase 6, Task 6.5)
    sVendorManager.loadVendorData();

    // Load gossip data (Phase 7, Task 7.5)
    sGossipManager.loadGossipData();

    // Load guild data (Phase 8, Task 8.6)
    sGuildManager.loadGuildsFromDatabase();

    // Initialize packet router
    sPacketRouter.initialize();

    // Initialize map manager with maps directory
    std::string mapsDir = sConfig.getMapsPath();
    if (!sMapManager.initialize(mapsDir)) {
        LOG_WARN("MapManager initialization failed (maps may not load)");
    }
    else
    {
        // Preload default start map to seed NPC spawns
        sMapManager.getMap(sMapManager.getDefaultStartMapId());
    }

    // Initialize world manager
    sWorldManager.initialize();

    // Start async saver
    sAsyncSaver.start();

    // Start game clock
    sGameClock.setTickRate(20); // 20 ticks per second
    sGameClock.start();

    // Create TCP listener
    sf::TcpListener listener;
    listener.setBlocking(false);

    if (listener.listen(sConfig.getServerPort()) != sf::Socket::Done) {
        LOG_ERROR("Failed to bind to port %d", sConfig.getServerPort());
        return 1;
    }
    LOG_INFO("Listening on port %d", sConfig.getServerPort());

    // Socket selector for efficient polling
    sf::SocketSelector selector;
    selector.add(listener);

    LOG_INFO("Server started. Press Ctrl+C to shutdown.");

    // Main server loop
    while (g_running) {
        try {
            // Update game clock
            bool shouldTick = sGameClock.tick();

            // Poll for network activity (short timeout to maintain responsiveness)
            if (selector.wait(sf::milliseconds(10))) {
                // Check for new connections
                if (selector.isReady(listener)) {
                    Session* session = sSessionManager.createSession();
                    if (session) {
                        auto socket = std::make_unique<SfSocket>(SfSocket::Type::ServerSide);
                        sf::TcpSocket* rawSocket = socket->getSocket();

                        if (listener.accept(*rawSocket) == sf::Socket::Done) {
                            rawSocket->setBlocking(false);
                            LOG_INFO("Session %u connected from %s",
                                     session->getId(),
                                     rawSocket->getRemoteAddress().toString().c_str());

                            session->setSocket(std::move(socket));
                            selector.add(*session->getSocket()->getSocket());
                        } else {
                            LOG_WARN("Failed to accept connection");
                            sSessionManager.removeSession(session->getId());
                        }
                    } else {
                        // Accept and immediately close to reject connection
                        sf::TcpSocket rejector;
                        listener.accept(rejector);
                        LOG_WARN("Connection rejected (server full)");
                    }
                }

                // Process existing sessions
                std::vector<uint32_t> sessionsToRemove;

                sSessionManager.forEachSession([&](Session& session) {
                    try {
                        SfSocket* socket = session.getSocket();
                        if (!socket || !socket->isConnected()) {
                            sessionsToRemove.push_back(session.getId());
                            return;
                        }

                        sf::TcpSocket* rawSocket = socket->getSocket();
                        if (selector.isReady(*rawSocket)) {
                            // Receive packets
                            std::vector<std::unique_ptr<StlBuffer>> packets;
                            socket->receive(packets);

                            // Check for disconnection
                            if (!socket->isConnected()) {
                                LOG_INFO("Session %u disconnected", session.getId());
                                selector.remove(*rawSocket);
                                sessionsToRemove.push_back(session.getId());
                                return;
                            }

                            // Process received packets
                            for (auto& packet : packets) {
                                if (packet->size() < 2) {
                                    LOG_WARN("Session %u: Malformed packet (size=%zu)",
                                             session.getId(), packet->size());
                                    continue;
                                }

                                uint16_t opcode;
                                *packet >> opcode;
                                sPacketRouter.dispatch(session, opcode, *packet);
                            }
                        }
                    } catch (const std::exception& e) {
                        LOG_ERROR("Session %u: Network error: %s", session.getId(), e.what());
                        sessionsToRemove.push_back(session.getId());
                    } catch (...) {
                        LOG_ERROR("Session %u: Unknown network error", session.getId());
                        sessionsToRemove.push_back(session.getId());
                    }
                });

                // Remove disconnected sessions (remove from selector first)
                for (uint32_t id : sessionsToRemove) {
                    Session* session = sSessionManager.getSession(id);
                    if (session && session->getSocket() && session->getSocket()->getSocket()) {
                        selector.remove(*session->getSocket()->getSocket());
                    }
                    sSessionManager.removeSession(id);
                }
            }

            // On each tick, update game systems
            if (shouldTick) {
                // Update session manager (timeout checks)
                sSessionManager.update();

                // Remove timed out sessions from selector
                sSessionManager.forEachSession([&](Session& session) {
                    if (session.shouldRemove()) {
                        if (session.getSocket() && session.getSocket()->getSocket()) {
                            selector.remove(*session.getSocket()->getSocket());
                        }
                    }
                });

                // Update world manager (updates all players) with error handling
                try {
                    sWorldManager.update(sGameClock.getDeltaTime());
                } catch (const std::exception& e) {
                    LOG_ERROR("World update error: %s", e.what());
                } catch (...) {
                    LOG_ERROR("Unknown world update error");
                }

                // Periodic status logging (every ~60 seconds)
                static uint64_t lastStatusTick = 0;
                if (sGameClock.getTickCount() - lastStatusTick >= 60ULL * sGameClock.getTickRate()) {
                    lastStatusTick = sGameClock.getTickCount();
                    LOG_INFO("Uptime: %s | Sessions: %zu | Ticks: %llu",
                             sGameClock.getUptimeString().c_str(),
                             sSessionManager.getSessionCount(),
                             static_cast<unsigned long long>(sGameClock.getTickCount()));
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Main loop exception: %s - server continues", e.what());
        } catch (...) {
            LOG_ERROR("Unknown main loop exception - server continues");
        }
    }

    // ========================================
    // Graceful Shutdown Sequence
    // ========================================
    LOG_INFO("Initiating graceful shutdown...");

    // 1. Stop accepting new connections
    listener.close();
    LOG_INFO("Stopped accepting connections");

    // 2. Disconnect all sessions with message
    sSessionManager.disconnectAll("Server shutting down");

    // 3. Remove remaining sessions from selector
    sSessionManager.forEachSession([&](Session& session) {
        if (session.getSocket() && session.getSocket()->getSocket()) {
            selector.remove(*session.getSocket()->getSocket());
        }
    });

    // 4. Shutdown world manager
    sWorldManager.shutdown();

    // 5. Flush and stop async saver
    sAsyncSaver.flush();
    sAsyncSaver.stop();

    // 6. Close database
    sDatabase.close();

    // 7. Final statistics
    LOG_INFO("Final uptime: %s", sGameClock.getUptimeString().c_str());
    LOG_INFO("Total ticks processed: %llu",
             static_cast<unsigned long long>(sGameClock.getTickCount()));

    LOG_INFO("Server stopped. Goodbye!");
    return 0;
}
