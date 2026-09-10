#include "game_world_replicated.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldReplicated::Update(const float deltaTime)
	{
		

	}

	void GameWorldReplicated::QueueGameCommand(GameCommand command)
	{
		m_incomingGameCommands.push(std::move(command));
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