#pragma once

#include "StlBuffer.h"
#include <memory>
#include <vector>

// Forward declaration
namespace sf { class TcpSocket; }

// Socket wrapper with packet buffering
// Handles partial sends and receives
class SfSocket
{
public:
    enum class Type
    {
        ClientSide,  // Client connecting to server
        ServerSide,  // Server accepting client
        SocketType_ClientSide = ClientSide,  // Legacy alias
        SocketType_ServerSide = ServerSide   // Legacy alias
    };

    // Main constructor
    SfSocket(Type type);

    // Legacy constructor for client usage (takes existing socket)
    SfSocket(std::shared_ptr<sf::TcpSocket> socket, Type type);

    ~SfSocket();

    // Send packet (handles partial sends)
    bool send(const StlBuffer& data);
    void sendPacket(StlBuffer data) { send(data); }  // Legacy alias

    // Receive packets (may return multiple)
    void receive(std::vector<std::unique_ptr<StlBuffer>>& output);
    void popReceived(std::vector<std::unique_ptr<StlBuffer>>& output) { receive(output); }  // Legacy alias

    // Connection state
    bool isConnected() const;
    bool connected() const { return isConnected(); }  // Legacy alias
    void disconnect();
    void cancel() { disconnect(); }  // Legacy alias
    std::string getRemoteAddress() const;

    // Update (process pending sends/receives) - returns false if connection lost
    bool update();

    // Access underlying socket (for polling)
    sf::TcpSocket* getSocket();

private:
    Type m_type;
    std::unique_ptr<sf::TcpSocket> m_ownedSocket;
    std::shared_ptr<sf::TcpSocket> m_sharedSocket;  // For legacy constructor
    sf::TcpSocket* m_socket = nullptr;  // Points to either owned or shared
    StlBuffer m_recvBuffer;
    StlBuffer m_sendBuffer;
};
