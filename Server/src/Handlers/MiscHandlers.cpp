// Miscellaneous Packet Handlers
// Task 2.6: Ping/Keepalive Handling

#include "stdafx.h"
#include "Handlers/MiscHandlers.h"
#include "Network/Session.h"
#include "Network/PacketRouter.h"
#include "Core/Logger.h"
#include "GamePacketBase.h"
#include "GamePacketServer.h"

namespace Handlers
{

void handlePing(Session& session, StlBuffer& data)
{
    (void)data;  // Ping packet has no data

    // Update the session's ping timestamp
    session.updateLastPing();

    // Build and send ping response
    GP_Mutual_Ping response;
    StlBuffer packet;
    uint16_t opcode = response.getOpcode();
    packet << opcode;
    response.pack(packet);

    session.sendPacket(packet);

    LOG_DEBUG("Session %u: Ping echoed", session.getId());
}

void registerMiscHandlers()
{
    // Client_Ping (0x01) - Primary ping from client, allowed in any connected state
    sPacketRouter.registerHandler(
        Opcode::Client_Ping,
        handlePing,
        SessionState::Connected,  // Required state
        true,                      // Allow higher states
        "Client_Ping"
    );

    // Also handle Mutual_Ping (0x00) for backwards compatibility
    sPacketRouter.registerHandler(
        Opcode::Mutual_Ping,
        handlePing,
        SessionState::Connected,
        true,
        "Mutual_Ping"
    );

    LOG_DEBUG("Misc handlers registered");
}

} // namespace Handlers
