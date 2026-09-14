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
		explicit Connection(SOCKET connectedSocket);

		~Connection();

		Connection(const Connection& connection) = delete;              // Copy construction: forbidden
		Connection& operator = (const Connection& connection) = delete; // Copy assignment: forbidden

		Connection(Connection&& connection) noexcept;                   // Move construction: permitted
		Connection& operator = (Connection&& connection) noexcept;      // Move assignment: permitted

		void ReceivePackets();
		bool PollPacket(Packet& packet);
		bool QueuePacket(Packet packet);
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