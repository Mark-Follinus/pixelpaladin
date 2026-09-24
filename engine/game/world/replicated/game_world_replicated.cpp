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

			if (const auto* state = std::get_if<PlayerStateEvent>(&event))
			{
				ApplyPlayerStateEvent(*state);
			}
			else if (const auto* joinedEvent = std::get_if<PlayerJoinedEvent>(&event))
			{
				ApplyPlayerJoinedEvent(*joinedEvent);
			}
			else if (const auto* disconnectEvent = std::get_if<PlayerDisconnectEvent>(&event))
			{
				ApplyPlayerDisconnectEvent(*disconnectEvent);
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

	void GameWorldReplicated::ApplyPlayerStateEvent(const PlayerStateEvent& event)
	{
		if (event.ID < 0 || static_cast<std::size_t>(event.ID) >= m_players.size())
		{
			std::printf("Error: Invalid player ID in game event: %i\n", event.ID);

			return;
		}

		const std::size_t playerIndex = static_cast<std::size_t>(event.ID);
		
		std::optional<PlayerCharacter>& playerSlot = m_players[playerIndex];

		if (!playerSlot.has_value())
		{
			// Stale player state from a disconnected client, or input received before joining completed.
			return;
		}

		playerSlot->State.Transform.Position = { event.PositionX, event.PositionY };
		playerSlot->State.Direction = event.Direction;
		playerSlot->State.Locomotion = event.Locomotion;
	}

	void GameWorldReplicated::ApplyPlayerJoinedEvent(const PlayerJoinedEvent& event)
	{
		if (event.ID < 0 || static_cast<std::size_t>(event.ID) >= m_players.size())
		{
			return;
		}

		auto& playerSlot = m_players[static_cast<std::size_t>(event.ID)];

		// Makes duplicate join events harmless.
		if (playerSlot.has_value())
		{
			return;
		}

		playerSlot.emplace();
		playerSlot->State.ID = event.ID;
		playerSlot->State.Transform.Position = { event.PositionX, event.PositionY };
	}

	void GameWorldReplicated::ApplyPlayerDisconnectEvent(const PlayerDisconnectEvent& event)
	{
		if (event.ID < 0 || static_cast<std::size_t>(event.ID) >= m_players.size())
		{
			return;
		}

		m_players[static_cast<std::size_t>(event.ID)].reset();
	}

}