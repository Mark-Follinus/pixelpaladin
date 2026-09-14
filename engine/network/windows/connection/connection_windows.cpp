#include "connection_windows.hpp"

// Standard library includes
#include <array>
#include <cstdio>
#include <cstring>
#include <utility>

// Windows library includes
#include <WS2tcpip.h>

// Compile time variables
constexpr std::size_t RECEIVE_CHUNK_SIZE_BYTES = 1024;
constexpr std::size_t PACKET_HEADER_SIZE_BYTES = sizeof(pldn::PacketHeader);
constexpr std::size_t MAX_PACKET_PAYLOAD_SIZE_BYTES = 4096;

namespace pldn
{

// ------------------------
// Public Constructors
// ------------------------ 

	Connection::Connection(SOCKET connectedSocket)
		: m_networkConnectionState(connectedSocket != INVALID_SOCKET ? NetworkConnectionState::Connected : NetworkConnectionState::Disconnected),
		  m_socket(connectedSocket)
	{}

	Connection::~Connection()
	{
		CloseSocket();
	}

	Connection::Connection(Connection&& connectionOther) noexcept
		: m_networkConnectionState  (std::exchange(connectionOther.m_networkConnectionState, NetworkConnectionState::Disconnected)),
		  m_socket                  (std::exchange(connectionOther.m_socket, INVALID_SOCKET)),
		  m_rawReceiveBuffer        (std::move(connectionOther.m_rawReceiveBuffer)),
		  m_rawSendBuffer           (std::move(connectionOther.m_rawSendBuffer)),
		  m_sendOffset              (std::exchange(connectionOther.m_sendOffset, 0)),
		  m_incomingPackets         (std::move(connectionOther.m_incomingPackets)),
		  m_outgoingPackets         (std::move(connectionOther.m_outgoingPackets))
	{}

// ------------------------
// Operator Overloads
// ------------------------

	Connection& Connection::operator=(Connection&& connectionOther) noexcept
	{
		if (this == &connectionOther)
		{
			return *this;
		}
			
		CloseSocket();

		m_networkConnectionState  = std::exchange(connectionOther.m_networkConnectionState, NetworkConnectionState::Disconnected);
		m_socket           = std::exchange(connectionOther.m_socket, INVALID_SOCKET);
		m_rawReceiveBuffer = std::move(connectionOther.m_rawReceiveBuffer);
		m_rawSendBuffer    = std::move(connectionOther.m_rawSendBuffer);
		m_sendOffset       = std::exchange(connectionOther.m_sendOffset, 0);
		m_incomingPackets  = std::move(connectionOther.m_incomingPackets);
		m_outgoingPackets  = std::move(connectionOther.m_outgoingPackets);

		connectionOther.m_socket = INVALID_SOCKET;
		connectionOther.m_networkConnectionState = NetworkConnectionState::Disconnected;
		connectionOther.m_sendOffset = 0;

		return *this;
	}

// ------------------------
// Public Methods
// ------------------------

	void Connection::ReceivePackets()
	{
		if (!IsConnected())
		{
			return;
		}

		bool connectionLost = false;
		std::array<std::byte, RECEIVE_CHUNK_SIZE_BYTES> receiveChunk{};

		// Getting all available data.
		while (true)
		{
			const int bytesReceived = recv(m_socket, reinterpret_cast<char*>(receiveChunk.data()), static_cast<int>(receiveChunk.size()), 0);

			if (bytesReceived > 0)
			{
				m_rawReceiveBuffer.insert(m_rawReceiveBuffer.end(), receiveChunk.begin(), receiveChunk.begin() + bytesReceived);
				continue;
			}

			if (bytesReceived == 0)
			{
				// Client performed and orderly disconnect.
				connectionLost = true;
				break;
			}

			if (bytesReceived == SOCKET_ERROR)
			{
				const int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					break; // Nothing available right now. Totally normal.
				}

				std::printf("ERROR: recv() failed: %i\n", error);
				Disconnect();
				return;
			}
		}

		// Parsing full packets.
		while (m_rawReceiveBuffer.size() >= PACKET_HEADER_SIZE_BYTES)
		{
			PacketHeader header{};
			std::memcpy(&header, m_rawReceiveBuffer.data(), PACKET_HEADER_SIZE_BYTES);

			if (header.PayloadSize > MAX_PACKET_PAYLOAD_SIZE_BYTES)
			{
				std::printf("ERROR: Packet payload too large.\n");
				Disconnect();
				return;
			}

			const size_t packetSize = PACKET_HEADER_SIZE_BYTES + header.PayloadSize;
			if (m_rawReceiveBuffer.size() < packetSize)
			{
				break; // Only part of the packet has arrived.
			}

			const auto payloadBegin = m_rawReceiveBuffer.begin() + PACKET_HEADER_SIZE_BYTES;
			const auto payloadEnd = m_rawReceiveBuffer.begin() + packetSize;

			Packet receivedPacket{};
			receivedPacket.Type = header.Type;
			receivedPacket.Payload.assign(payloadBegin, payloadEnd);

			m_incomingPackets.push(std::move(receivedPacket));
			m_rawReceiveBuffer.erase(m_rawReceiveBuffer.begin(), m_rawReceiveBuffer.begin() + packetSize);
		}

		if (connectionLost)
		{
			if (!m_rawReceiveBuffer.empty())
			{
				std::printf("WARNING: Peer disconnected with incomplete packet data.\n");
			}

			Disconnect();
		}
	}

	bool Connection::PollPacket(Packet& packet)
	{
		if (m_incomingPackets.empty())
		{
			return false;
		}

		packet = std::move(m_incomingPackets.front());
		m_incomingPackets.pop();

		return true;
	}

	bool Connection::QueuePacket(Packet packet)
	{
		if (!IsConnected())
		{
			return false;
		}

		if (packet.Type == PacketType::Invalid)
		{
			std::printf("ERROR: Cannot queue an invalid packet.\n");
			return false;
		}

		if (packet.Payload.size() > MAX_PACKET_PAYLOAD_SIZE_BYTES)
		{
			std::printf("ERROR: Packet payload too large.\n");
			return false;
		}

		m_outgoingPackets.push(std::move(packet));

		return true;
	}

	void Connection::FlushPackets()
	{
		if (!IsConnected())
		{
			return;
		}

		while (!m_outgoingPackets.empty())
		{
			Packet& packet = m_outgoingPackets.front();
			if (packet.Payload.size() > MAX_PACKET_PAYLOAD_SIZE_BYTES)
			{
				std::printf("ERROR: Packet payload too large.\n");
				m_outgoingPackets.pop();
				continue;
			}

			PacketHeader header{packet.Type, static_cast<std::uint16_t>(packet.Payload.size())};

			const std::size_t previousSize = m_rawSendBuffer.size();
			const std::size_t currentPacketSize = PACKET_HEADER_SIZE_BYTES + packet.Payload.size();

			m_rawSendBuffer.resize(previousSize + currentPacketSize);
			std::memcpy(m_rawSendBuffer.data() + previousSize, &header, PACKET_HEADER_SIZE_BYTES);

			if (!packet.Payload.empty())
			{
				std::memcpy(m_rawSendBuffer.data() + previousSize + PACKET_HEADER_SIZE_BYTES, packet.Payload.data(), packet.Payload.size());
			}

			m_outgoingPackets.pop();
		}

		while (m_sendOffset < m_rawSendBuffer.size())
		{
			const std::size_t bytesRemaining = m_rawSendBuffer.size() - m_sendOffset;

			const int bytesSent = send(m_socket, reinterpret_cast<const char*>(m_rawSendBuffer.data() + m_sendOffset), static_cast<int>(bytesRemaining), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				const int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK)
				{
					return; // Retain the unsent bytes for the next tick.
				}

				std::printf("ERROR: send() failed: %i\n", error);
				Disconnect();
				return;
			}

			if (bytesSent == 0)
			{
				std::printf("ERROR: send() made no progress.\n");
				Disconnect();
				return; // Prevent an infinite loop if no progress was made.
			}

			m_sendOffset += static_cast<std::size_t>(bytesSent);
		}
		
		m_rawSendBuffer.clear();
		m_sendOffset = 0;
	}

	void Connection::Disconnect()
	{
		m_rawReceiveBuffer.clear();
		m_rawSendBuffer.clear();
		m_sendOffset = 0;

		m_incomingPackets = {};
		m_outgoingPackets = {};

		CloseSocket();

		// Maybe in the future...
			//   logs errors;
			//   clears packet queues;
			//   changes other state;
			//   returns a status.
	}

// ------------------------
// Private Methods
// ------------------------

	void Connection::CloseSocket() noexcept
	{
		if (m_socket != INVALID_SOCKET)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		m_networkConnectionState = NetworkConnectionState::Disconnected;
	}

} // namespace pldn