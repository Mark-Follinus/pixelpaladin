#pragma once

// Standard library includes
#include <cstdint>
#include <string>
#include <queue>
#include <optional>

// Windows library includes
#include <WinSock2.h>

// Engine includes
#include "game/commands/game_command.hpp"
#include "input/commands/input_command.hpp"
#include "./network/windows/connection/connection_windows.hpp"

namespace pldn
{
	enum class NetworkClientState
	{
		Disconnected,
		Connecting,
		Connected
	};

	class NetworkClient
	{
	public:
		void ConnectToServer(const std::string& ipAddress, const uint16_t port);
		void DisconnectFromServer();
		bool IsConnected() const;

		void Send();
		void Receive();

		void QueueInputCommand(InputCommand command);
		bool PollGameCommand(GameCommand& command);

	private:
		NetworkClient();
		~NetworkClient();

		void CheckConnection();
		void ClosePendingSocket();

	private:
		friend class Engine;
		friend class ClientEngine;

		NetworkClientState m_connectionState = NetworkClientState::Disconnected;

		SOCKET m_pendingSocket = INVALID_SOCKET; // While in the process of joining, socket stored outside of connection.
		std::optional<Connection> m_connection;

		std::queue<InputCommand> m_outgoingInputCommands = {};
		std::queue<GameCommand> m_incomingGameCommands = {};
	};
}



