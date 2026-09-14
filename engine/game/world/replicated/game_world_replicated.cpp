#include "game_world_replicated.hpp"

// Standard library includes
#include <utility>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldReplicated::Update(const float deltaTime)
	{
		

	}

	void GameWorldReplicated::QueueGameEvent(GameEvent command)
	{
		m_incomingGameEvents.push(std::move(command));
	}

// ------------------------
// Private Constructors
// ------------------------

	GameWorldReplicated::~GameWorldReplicated()
	{

	}

// ------------------------
// Private Methods
// ------------------------

}