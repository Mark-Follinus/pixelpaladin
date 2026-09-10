#pragma once

// Standard library includes
#include <cstddef>
#include <queue>
#include <vector>

// Windows library includes
#include <WinSock2.h>

// Engine includes
#include "./network/data/packet.hpp"

namespace pldn
{
	enum class NetworkConnectionState
	{
		Disconnected,
		Connecting,
		Connected
	};

	class Connection
	{
	public:

		// Server
		// 1) [Networking-System] WSA init.
		// 2) [Server-Interface ] Creates a socket.
		// 3) [Server-Interface ] Binds socket to port.
		// 4) [Server-Interface ] Listen for clients.
		// 5) [Server-Interface ] Accept clients.
		// 6)              [THIS] Send / Receive from clients.
		// 7) [Server-Interface ] Kick Client.

		// Client
		// 1) [Networking-System] WSA init.
		// 2) [Client-Interface ] Creates a socket.
		// 3) [Client-Interface ] Join server.
		// 4)              [THIS] Send / Receive from server.
		// 5) [Client-Interface ] Disconnect from server.
		explicit Connection(SOCKET connectedSocket);

		~Connection();

		Connection(const Connection& connection) = delete;              // Copy construction: forbidden
		Connection& operator = (const Connection& connection) = delete; // Copy assignment: forbidden

		Connection(Connection&& connection) noexcept;                   // Move construction: permitted
		Connection& operator = (Connection&& connection) noexcept;      // Move assignment: permitted

		void ReceivePackets();
		bool PollPacket(Packet& packet); // While loop later, returns the front of the queue brev...
		void QueuePacket(Packet packet);
		void FlushPackets();

		void Disconnect();  
		bool IsConnected() const { return m_networkConnectionState == NetworkConnectionState::Connected; }
		NetworkConnectionState ConnectionState() const { return m_networkConnectionState; }

	private:
		void CloseSocket() noexcept;

	private:
		NetworkConnectionState m_networkConnectionState = NetworkConnectionState::Disconnected;

		SOCKET m_socket = INVALID_SOCKET;

		std::vector<std::byte> m_rawReceiveBuffer = {};
		std::vector<std::byte> m_rawSendBuffer = {};
		std::size_t m_sendOffset = 0;

		std::queue<Packet> m_incomingPackets = {};
		std::queue<Packet> m_outgoingPackets = {};
	};

} // namespace pldn