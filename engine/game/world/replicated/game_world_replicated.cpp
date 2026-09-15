#include "game_world_replicated.hpp"

// Standard library includes
#include <cstdio>
#include <utility>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldReplicated::Update(const float deltaTime)
	{
		while (!m_incomingGameEvents.empty())
		{
			GameEvent event = std::move(m_incomingGameEvents.front());
			m_incomingGameEvents.pop();

			switch (event.Type)
			{
				case GameEventType::PlayerState:
				{
					ApplyPlayerStateEvent(event);
					break;
				}

				default: break;
			}
		}
	}

	void GameWorldReplicated::QueueGameEvent(GameEvent event)
	{
		m_incomingGameEvents.push(std::move(event));
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

	void GameWorldReplicated::ApplyPlayerStateEvent(const GameEvent& event)
	{
		if (event.ID < 0 || static_cast<std::size_t>(event.ID) >= m_players.size())
		{
			std::printf("Error: Invalid player ID in game event: %i\n", event.ID);

			return;
		}

		const std::size_t playerIndex = static_cast<std::size_t>(event.ID);
		
		std::optional<PlayerState>& playerSlot = m_players[playerIndex];

		if (!playerSlot.has_value())
		{
			playerSlot.emplace();
			playerSlot->ID = event.ID;
		}

		playerSlot->Position = { event.PositionX, event.PositionY };
	}

}