#pragma once

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

	enum class ConnectionState
	{
		Disconnected,
		Connecting,
		Connected
	};

	struct NetworkConfig
	{
		// Address
		// Port
		// Other startup configuration
	};

	NetworkRole GetRole() const { return m_role; }
	ConnectionState GetConnectionState() const { return m_connectionState; }

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

	NetworkRole m_role = NetworkRole::None;
	ConnectionState m_connectionState = ConnectionState::Disconnected;
	NetworkConfig m_config = {};
};