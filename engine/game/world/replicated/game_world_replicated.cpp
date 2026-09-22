#include "game_world_replicated.hpp"

// Standard library includes
#include <cstdio>
#include <utility>
#include <variant>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldReplicated::Update(const float /* deltaTime */)
	{
		while (!m_incomingGameEvents.empty())
		{
			GameEvent event = std::move(m_incomingGameEvents.front());
			m_incomingGameEvents.pop();

			const PlayerStateEvent* playerStateEvent = std::get_if<PlayerStateEvent>(&event);
			if (playerStateEvent == nullptr)
			{
				continue;
			}

			ApplyPlayerStateEvent(*playerStateEvent);
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

	void GameWorldReplicated::ApplyPlayerStateEvent(const PlayerStateEvent& event)
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

		playerSlot->Transform.Position = { event.PositionX, event.PositionY };
	}

}