#pragma once

// Standard library includes
#include <array>
#include <queue>
#include <optional>

// Engine includes
#include "input/commands/input_command.hpp"
#include "game/commands/game_command.hpp"

#include "game/player_state.hpp"

// Compile time variables
constexpr size_t MAX_PLAYERS_ALLOWED = 4;

namespace pldn
{
	class GameWorldAuthoritative
	{

	public:
		void Update(const float deltaTime);

		void QueueInputCommand(ClientInputCommand command);
		bool PollGameCommand(ClientGameCommand& command);

	private:
		GameWorldAuthoritative();
		~GameWorldAuthoritative();

	private:
		friend class Engine;
		friend class ServerEngine;

		std::array<std::optional<PlayerState>, MAX_PLAYERS_ALLOWED> m_players = {};

		std::queue<ClientInputCommand> m_incomingInputCommands = {};
		std::queue<ClientGameCommand> m_outgoingGameCommands = {};

		uint32_t m_serverTick = 0;
	};

} // namespace pldn