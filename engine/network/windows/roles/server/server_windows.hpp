#pragma once

// Standard library includes
#include <cstdint>
#include <string>
#include <array>
#include <queue>
#include <optional>

// Windows library includes
#include <WinSock2.h>

// Engine includes
#include "game/events/game_event.hpp"
#include "input/commands/input_command.hpp"
#include "network/data/network_types.hpp"
#include "network/data/network_config.hpp"
#include "network/windows/connection/connection_windows.hpp"

// Compile time variables
constexpr size_t MAX_CLIENT_CONNECTIONS = 4;

namespace pldn
{
	struct ClientConnection
	{
		ClientConnection(int8_t id, SOCKET socket)
			: ID(id), ConnectedPeer(socket) {}

		ClientID ID = -1;
		Connection ConnectedPeer;
	};

	class NetworkServer
	{
	public:
		bool Start(); // Returns true if successfully started. False if it went to shit.
		void Stop();

		void Receive();
		void Send();

		bool PollInputCommand(ClientInputCommand& command);
		void QueueGameEvent(GameEvent event);

	private:
		NetworkServer(const std::string& ipAddress, uint16_t port);
		~NetworkServer();

		void AcceptClients();

	private:
		friend class Engine;
		friend class ServerEngine;

		NetworkConfig m_networkConfig = {};

		SOCKET m_listeningSocket = INVALID_SOCKET;
		std::array<std::optional<ClientConnection>, MAX_CLIENT_CONNECTIONS> m_connections = {};

		std::queue<ClientInputCommand> m_incomingInputCommands = {};
		std::queue<GameEvent> m_outgoingGameEvents = {};
	};
}