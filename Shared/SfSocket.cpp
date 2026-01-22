// SfSocket - SFML Socket wrapper with packet buffering
// TODO: Implement fully in Task 1.9

#include "SfSocket.h"
#include <SFML/Network.hpp>

SfSocket::SfSocket(Type type)
    : m_type(type)
    , m_ownedSocket(std::make_unique<sf::TcpSocket>())
{
    m_socket = m_ownedSocket.get();
    m_socket->setBlocking(false);
}

SfSocket::SfSocket(std::shared_ptr<sf::TcpSocket> socket, Type type)
    : m_type(type)
    , m_sharedSocket(socket)
{
    m_socket = m_sharedSocket.get();
}

SfSocket::~SfSocket()
{
    disconnect();
}

bool SfSocket::update()
{
    if (!isConnected())
        return false;

    // Try to receive any pending data (this just appends to buffer)
    std::vector<std::unique_ptr<StlBuffer>> dummy;
    receive(dummy);  // Ignoring received packets in update, caller should call popReceived

    return isConnected();
}

bool SfSocket::send(const StlBuffer& data)
{
    if (!m_socket || !isConnected())
        return false;

    // Build packet with size header
    StlBuffer packet;
    packet.build(StlBuffer(std::vector<uint8_t>(data.data(), data.data() + data.size())));

    size_t sent = 0;
    auto status = m_socket->send(packet.data(), packet.size(), sent);
    return (status == sf::Socket::Done || status == sf::Socket::Partial);
}

void SfSocket::receive(std::vector<std::unique_ptr<StlBuffer>>& output)
{
    if (!m_socket)
        return;

    // Receive into buffer
    uint8_t tempBuf[4096];
    size_t received = 0;
    auto status = m_socket->receive(tempBuf, sizeof(tempBuf), received);

    if (status == sf::Socket::Done && received > 0) {
        // Append to receive buffer
        for (size_t i = 0; i < received; ++i) {
            m_recvBuffer << tempBuf[i];
        }
    }

    // Extract complete packets
    // Wire format: [4 bytes: payload size (uint32 LE)] [2 bytes: opcode] [payload]
    while (m_recvBuffer.size() >= 6) {  // Minimum: 4 bytes size + 2 bytes opcode
        // Peek at payload size (4 bytes, little-endian)
        uint32_t payloadSize = static_cast<uint32_t>(m_recvBuffer.data()[0]) |
                               (static_cast<uint32_t>(m_recvBuffer.data()[1]) << 8) |
                               (static_cast<uint32_t>(m_recvBuffer.data()[2]) << 16) |
                               (static_cast<uint32_t>(m_recvBuffer.data()[3]) << 24);

        // Validate payload size (must be at least 2 bytes for opcode)
        if (payloadSize < 2) {
            // Invalid packet - discard byte and try again
            m_recvBuffer.eraseFront(1);
            continue;
        }

        // Sanity check: don't allow packets > 1MB
        if (payloadSize > 1000000) {
            // Likely garbage - disconnect
            disconnect();
            return;
        }

        // Check if we have the full packet (4-byte header + payload)
        if (m_recvBuffer.size() < 4 + payloadSize)
            break;  // Incomplete packet

        // Extract packet payload (skip 4-byte size header)
        auto pkt = std::make_unique<StlBuffer>(
            std::vector<uint8_t>(m_recvBuffer.data() + 4, m_recvBuffer.data() + 4 + payloadSize));
        output.push_back(std::move(pkt));

        m_recvBuffer.eraseFront(4 + payloadSize);
    }
}

bool SfSocket::isConnected() const
{
    return m_socket && m_socket->getRemoteAddress() != sf::IpAddress::None;
}

void SfSocket::disconnect()
{
    if (m_socket) {
        m_socket->disconnect();
    }
}

sf::TcpSocket* SfSocket::getSocket()
{
    return m_socket;
}

std::string SfSocket::getRemoteAddress() const
{
    if (m_socket) {
        return m_socket->getRemoteAddress().toString() + ":" + std::to_string(m_socket->getRemotePort());
    }
    return "unknown";
}
