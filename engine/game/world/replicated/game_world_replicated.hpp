#pragma once

// Standard library includes
#include <queue>

// Engine includes
#include "game/commands/game_command.hpp"

#include "game/player_state.hpp"

namespace pldn
{
	
	class GameWorldReplicated
	{

	public:
		void Update(const float deltaTime);
		void QueueGameCommand(GameCommand command);

	private:
		GameWorldReplicated() = default;
		~GameWorldReplicated();

	private:
		friend class Engine;
		friend class ClientEngine;

		std::queue<GameCommand> m_incomingGameCommands = {};
	};

} // namespace pldn
