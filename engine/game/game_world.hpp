#pragma once

#include <vector>

// Engine Includes
#include "data/player_data.hpp"

namespace pldn
{
	struct InputCommand;

	class GameWorld
	{

	public:

	private:
		GameWorld();
		~GameWorld();

		void Update(const InputCommand& command, const const float deltaTime);

	private:
		friend class Engine;

		PlayerDataMovement playerData;
	};


} // namespace pldn