#include "game_world_authoritative.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldAuthoritative::Update(const float deltaTime)
	{


	}

	void GameWorldAuthoritative::QueueInputCommand(ClientInputCommand command) 
	{
		m_incomingInputCommands.push(std::move(command));
	}

	bool GameWorldAuthoritative::PollGameCommand(ClientGameCommand& command)
	{
		if (m_outgoingGameCommands.empty())
		{
			return false;
		}

		command = std::move(m_outgoingGameCommands.front());

		m_outgoingGameCommands.pop();
		return true;
	}

// ------------------------
// Private Constructors
// ------------------------

	GameWorldAuthoritative::GameWorldAuthoritative()
	{

	}

	GameWorldAuthoritative::~GameWorldAuthoritative()
	{

	}

// ------------------------
// Private Methods
// ------------------------


} // namespace pldn