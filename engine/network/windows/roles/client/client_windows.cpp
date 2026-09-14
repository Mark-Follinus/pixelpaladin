#include "client_windows.hpp"

// Standard library includes
#include <cstdio>
#include <utility>

// Windows library includes
#include <WS2tcpip.h>

// Engine includes
#include "./network/data/network_codec.hpp"

// Compile time variables
constexpr u_long NON_BLOCKING_SOCKET_COMMAND = 1ul;

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void NetworkClient::ConnectToServer(const std::string& ipAddress, const uint16_t port)
	{
		if (m_connectionState != NetworkClientState::Disconnected)
		{
			std::printf("Client: Connection attempt already active.\n");
			return;
		}

		if ((m_pendingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		{
			const int error = WSAGetLastError();
			std::printf("Client: Socket configuration failed: %i\n", error);

			return;
		}

		u_long nonBlocking = NON_BLOCKING_SOCKET_COMMAND;
		if (ioctlsocket(m_pendingSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
		{
			const int error = WSAGetLastError();
			std::printf("Client: Failed to make socket non-blocking: %i\n", error);

			closesocket(m_pendingSocket);
			m_pendingSocket = INVALID_SOCKET;

			return;
		}

		sockaddr_in serverAddress{};
			serverAddress.sin_family = AF_INET;
			serverAddress.sin_port = htons(port);

		if (InetPtonA(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr) != 1)
		{
			const int error = WSAGetLastError();
			std::printf("Client: InetPton failed: %i\n", error);

			closesocket(m_pendingSocket);
			m_pendingSocket = INVALID_SOCKET;

			return;
		}
		else if (connect(m_pendingSocket, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
		{
			const int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK)
			{
				m_connectionState = NetworkClientState::Connecting;
				std::printf("Client: Connection in progress.\n");
			}
			else 
			{
				std::printf("Client: Connecting to server failed: %i\n", error);

				closesocket(m_pendingSocket);
				m_pendingSocket = INVALID_SOCKET;

				return;
			}
		}
		else 
		{
			m_connectionState = NetworkClientState::Connected;
			m_connection.emplace(std::exchange(m_pendingSocket, INVALID_SOCKET));

			std::printf("Client: Connected to Server.\n");
		}
	}

	void NetworkClient::DisconnectFromServer()
	{
		ClosePendingSocket();

		if (m_connection.has_value())
		{
			m_connection->Disconnect();
			m_connection.reset();
		}

		m_outgoingInputCommands = {};
		m_incomingGameEvents = {};

		m_connectionState = NetworkClientState::Disconnected;
	}

	bool NetworkClient::IsConnected() const
	{
		return m_connectionState == NetworkClientState::Connected && m_connection.has_value() && m_connection->IsConnected();
	}

	void NetworkClient::Send()
	{
		if (!IsConnected())
		{
			return;
		}

		while (!m_outgoingInputCommands.empty())
		{
			m_connection->QueuePacket(SerializeInputCommand(m_outgoingInputCommands.front()));
			m_outgoingInputCommands.pop();
		}

		m_connection->FlushPackets();

		if (!m_connection->IsConnected())
		{
			m_connection.reset();
			m_connectionState = NetworkClientState::Disconnected;
		}
	}

	void NetworkClient::Receive()
	{		
		if (m_connectionState == NetworkClientState::Connecting)
		{
			CheckConnection();

			if (m_connectionState != NetworkClientState::Connected)
			{
				return;
			}
		}

		if (!m_connection.has_value())
		{
			return;
		}

		m_connection->ReceivePackets();

		bool removeConnection = false;
		Packet packet{};

		while (m_connection->PollPacket(packet))
		{
			switch (packet.Type)
			{
				case PacketType::GameEvent:
				{
					GameEvent command{};

					if (!DeserializeGameEvent(packet, command))
					{
						std::printf("Client: Invalid Game Event packet.\n");

						m_connection->Disconnect();
						removeConnection = true;
						break;
					}

					m_incomingGameEvents.push(std::move(command));
					break;
				}

				default:
				{
					std::printf("Client: Unexpected packet type.\n");
					break;
				}
			}

			if (removeConnection)
			{
				break;
			}
		}

		if (!m_connection->IsConnected())
		{
			m_connection.reset();
			m_connectionState = NetworkClientState::Disconnected;
		}
	}

	void NetworkClient::QueueInputCommand(InputCommand command)
	{
		if (!IsConnected())
		{
			return;
		}

		m_outgoingInputCommands.push(std::move(command));
	}

	bool NetworkClient::PollGameEvent(GameEvent& event)
	{
		if (m_incomingGameEvents.empty())
		{
			return false;
		}

		event = std::move(m_incomingGameEvents.front());

		m_incomingGameEvents.pop();
		return true;
	}

// ------------------------
// Private Constructors
// ------------------------ 

	NetworkClient::NetworkClient() {}

	NetworkClient::~NetworkClient()
	{
		DisconnectFromServer();
	}

// ------------------------
// Private Methods
// ------------------------

	void NetworkClient::CheckConnection()
	{
		if (m_pendingSocket == INVALID_SOCKET)
		{
			m_connectionState = NetworkClientState::Disconnected;
			return;
		}

		fd_set writeSet{};
		fd_set exceptionSet{};

		FD_ZERO(&writeSet);
		FD_ZERO(&exceptionSet);

		FD_SET(m_pendingSocket, &writeSet);
		FD_SET(m_pendingSocket, &exceptionSet);

		timeval timeout{};
		timeout.tv_sec = 0;  // Time out in seconds.
		timeout.tv_usec = 0; // Time out in microseconds.

		const int result = select(0, nullptr, &writeSet, &exceptionSet, &timeout);
		if (result == SOCKET_ERROR)
		{
			const int error = WSAGetLastError();
			std::printf("Client: Select failed: %i\n", error);

			m_connectionState = NetworkClientState::Disconnected;
			ClosePendingSocket();

			return;
		}

		if (result == 0)
		{
			return; // Still connecting.
		}

		if (!FD_ISSET(m_pendingSocket, &writeSet) &&
			!FD_ISSET(m_pendingSocket, &exceptionSet))
		{
			return;
		}

		int socketError = 0;
		int errorSize = sizeof(socketError);

		if (getsockopt(m_pendingSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &errorSize) == SOCKET_ERROR)
		{
			socketError = WSAGetLastError();
		}

		if (socketError != 0)
		{
			std::printf("Client: Connection failed: %i\n", socketError);

			ClosePendingSocket();
			m_connectionState = NetworkClientState::Disconnected;

			return;
		}

		m_connection.emplace(std::exchange(m_pendingSocket, INVALID_SOCKET));
		m_connectionState = NetworkClientState::Connected;

		std::printf("Client: Connected to server!\n");
	}

	void NetworkClient::ClosePendingSocket()
	{
		if (m_pendingSocket != INVALID_SOCKET)
		{
			closesocket(m_pendingSocket);
			m_pendingSocket = INVALID_SOCKET;
		}
	}

}