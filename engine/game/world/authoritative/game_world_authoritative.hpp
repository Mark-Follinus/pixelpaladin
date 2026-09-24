#pragma once

// Standard library includes
#include <array>
#include <queue>
#include <optional>

// Engine includes
#include "network/windows/connection/connection_windows.hpp"
#include "game/constants/game_constants.hpp"
#include "game/replicated/player_state.hpp"

// Engine event & command includes
#include "input/commands/input_command.hpp"
#include "network/events/connection_events.hpp"
#include "game/entities/player_character.hpp"
#include "game/events/game_event.hpp"

namespace pldn
{
	class GameWorldAuthoritative
	{
		//struct PlayerStateAuthoritative
		//{
		//	PlayerState State = {};
		//	InputCommand CurrentInput = {};
		//};

	public:
		void Update(const float deltaTime);

		void QueueConnectionChange(ClientConnectionChangeEvent event);
		void QueueInputCommand(ClientInputCommand command);

		bool PollGameEvent(GameEvent& event);

	private:
		GameWorldAuthoritative();
		~GameWorldAuthoritative();

		void ProcessConnectionEvents();
		void ProcessInputCommands();

		void MovePlayers(const float deltaTime);
		bool AddPlayer(ClientID id);
		bool RemovePlayer(ClientID id);

		void BroadcastPlayerJoinedEvents();
		void BroadcastPlayerDisconnectEvent(ClientID id);

		void GeneratePlayerStateGameEvents();

	private:
		friend class Engine;
		friend class ServerEngine;

		std::array<std::optional<PlayerCharacter>, MAX_PLAYERS_ALLOWED> m_players = {};
		std::array<InputCommand, MAX_PLAYERS_ALLOWED> m_currentInputs = {};

		std::queue<ClientConnectionChangeEvent> m_incomingConnectionEvents = {};
		std::queue<ClientInputCommand> m_incomingInputCommands = {};
		std::queue<GameEvent> m_outgoingGameEvents = {};

		uint32_t m_serverTick = 0;
	};

} // namespace pldn