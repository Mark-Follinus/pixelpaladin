#pragma once

// Standard library includes
#include <array>
#include <queue>
#include <optional>

// Engine includes
#include "input/commands/input_command.hpp"
#include "game/constants/game_constants.hpp"
#include "game/events/game_event.hpp"
#include "game/game_objects/player_state.hpp"

namespace pldn
{
	class GameWorldAuthoritative
	{
		struct PlayerStateAuthoritative
		{
			PlayerState State = {};
			InputCommand CurrentInput = {};
		};

	public:
		void Update(const float deltaTime);

		void QueueInputCommand(ClientInputCommand command);
		bool PollGameEvent(GameEvent& command);

	private:
		GameWorldAuthoritative();
		~GameWorldAuthoritative();

		void ProcessInputCommands();
		void MovePlayers(const float deltaTime);
		void GenerateContinuousGameEvents(); // Game events that should be send each tick (stuff like player positions)

	private:
		friend class Engine;
		friend class ServerEngine;

		std::array<std::optional<PlayerStateAuthoritative>, MAX_PLAYERS_ALLOWED> m_players = {};

		std::queue<ClientInputCommand> m_incomingInputCommands = {};
		std::queue<GameEvent> m_outgoingGameEvents = {};

		uint32_t m_serverTick = 0;
	};

} // namespace pldn