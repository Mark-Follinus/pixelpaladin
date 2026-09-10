#pragma once

// Standard library includes
#include <vector>
#include <array>

// Windows library includes
#include <WinSock2.h>

namespace pldn
{

	// Forward Declarations
	struct InputCommand;
	class Input;

	class Networking
	{
	public:
		enum class NetworkRole
		{
			None,   // No multiplayer.
			Client, // Player only.
			Server, // Server only.
			Host    // Player and server.
		};

		enum class NetworkConnectionState
		{
			Disconnected,
			Connecting,
			Connected
		};

		struct NetworkConfig
		{
			short ServerPort = 8080;
			int WinSockVersionMajor = 2;
			int WinSockVersionMinor = 2;
			const char* IPAddress = "127.0.0.1";
			const int MaximumConnections = 4;
		};

		struct NetworkConnection
		{
			SOCKET Socket = INVALID_SOCKET;
			std::vector<char> RawReceiveBuffer = {};
			std::vector<char> RawSendBuffer = {};
			NetworkConnectionState State = NetworkConnectionState::Disconnected;
		};

		NetworkRole GetRole() const { return m_role; }

	private:
		Networking(const NetworkRole role, const NetworkConfig& config);
		~Networking();

		void InitializeAsNone();
		void InitializeAsClient();
		void InitializeAsServer();
		void InitializeAsHost();

		void Update(const InputCommand& command);
		void UpdateClient(const InputCommand& command);
		void UpdateServer();
		void UpdateHost();

		void CheckServerConnection();
		void AcceptClientConnections();

		void DisconnectFromServer(NetworkConnection& connection);

		void ParseOutgoingData(NetworkConnection& connection);
		void ParseOutgoingData(NetworkConnection& connection, const InputCommand& command);
		void SendData(NetworkConnection& connection);

		void ReceiveData(NetworkConnection& connection);
		void ParseIncomingData(NetworkConnection& connection);

	private:
		friend class Engine;

		// General data members
		NetworkConfig m_config = {};
		NetworkRole m_role = NetworkRole::None;
		WSADATA m_wsaData = {};
		bool m_wsaInitialized = false;

		// Client role data members
		NetworkConnection m_clientToServerConnection;

		// Server role data members
		SOCKET m_serverListenSocket = INVALID_SOCKET;
		std::array<NetworkConnection, 4> m_serverToClientConnections;
	};

} // namespace pldn