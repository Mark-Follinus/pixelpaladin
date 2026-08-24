#pragma once

// Standard library includes
#include <vector>

// Windows library includes
#include <WinSock2.h>

#include "primitive_types.hpp"

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

	struct NetworkConfig
	{
		i16 ServerPort = 8080;

		i32 WinSockVersionMajor = 2;
		i32 WinSockVersionMinor = 2;

		const char* IPAddress = "127.0.0.1";

		const i32 MaximumConnections = 1;

		// Address
		// Port
		// Other startup configuration
	};

	NetworkRole GetRole() const { return m_role; }

private:
	Networking(const NetworkRole role, const NetworkConfig& config);
	~Networking();

	void InitializeAsNone();
	void InitializeAsClient();
	void InitializeAsServer();
	void InitializeAsHost();

	void Update();
	void UpdateClient();
	void UpdateServer();

private:
	friend class Engine;

	NetworkConfig m_config = {};
	NetworkRole m_role = NetworkRole::None;

	SOCKET m_serverSocket = INVALID_SOCKET;   // If client, handle to connect to server.
	SOCKET m_listenSocket = INVALID_SOCKET;   // If server, handle to listen for connections.
	std::vector<SOCKET> m_clientSockets = {}; // If server, handles for clients.
};