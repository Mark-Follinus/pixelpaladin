#include "game_world_authoritative.hpp"

// Standard library includes
#include <utility>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void GameWorldAuthoritative::Update(const float deltaTime)
	{
		ProcessConnectionEvents(); // Clients joining or leaving.

		ProcessInputCommands();

		MovePlayers(deltaTime);
		// ResolvePlayerActions();
		// ResolveCollisions();

		// Continuous state replication is generated at the end of the tick. (Every tick send event).
		GeneratePlayerStateGameEvents();

		m_serverTick++;
	}

	void GameWorldAuthoritative::QueueConnectionChange(ClientConnectionChangeEvent event)
	{
		m_incomingConnectionEvents.push(event);
	}

	void GameWorldAuthoritative::QueueInputCommand(ClientInputCommand command) 
	{
		m_incomingInputCommands.push(std::move(command));
	}

	bool GameWorldAuthoritative::PollGameEvent(GameEvent& command)
	{
		if (m_outgoingGameEvents.empty())
		{
			return false;
		}

		command = std::move(m_outgoingGameEvents.front());

		m_outgoingGameEvents.pop();
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

	void GameWorldAuthoritative::ProcessConnectionEvents()
	{
		while (!m_incomingConnectionEvents.empty())
		{
			const ClientConnectionChangeEvent changeEvent = m_incomingConnectionEvents.front();

			m_incomingConnectionEvents.pop();

			switch (changeEvent.Type)
			{
				case ClientConnectionChangeEventType::Connected:
				{
					if (AddPlayer(changeEvent.ID))
					{
						BroadcastPlayerJoinedEvents();
					}

					break;
				}

				case ClientConnectionChangeEventType::Disconnected:
				{
					if (RemovePlayer(changeEvent.ID))
					{
						BroadcastPlayerDisconnectEvent(changeEvent.ID);
					}

					break;
				}
			}
		}
	}

	void GameWorldAuthoritative::ProcessInputCommands()
	{
		while (!m_incomingInputCommands.empty())
		{
			ClientInputCommand incoming = std::move(m_incomingInputCommands.front());

			m_incomingInputCommands.pop();

			const ClientID clientID = incoming.ClientID;

			if (clientID < 0 || static_cast<std::size_t>(clientID) >= m_players.size())
			{
				continue;
			}

			std::optional<PlayerStateAuthoritative>& slot = m_players[static_cast<std::size_t>(clientID)];

			if (!slot.has_value())
			{
				// Stale input from a disconnected client, or input received before joining completed.
				continue;
			}

			PlayerStateAuthoritative& player = *slot;
			const InputCommand& command = incoming.Command;

			if (command.SequenceNumber <= player.State.LastProcessedInputSequence)
			{
				continue;
			}

			player.State.LastProcessedInputSequence = command.SequenceNumber;
			player.CurrentInput = command;
		}
	}

	void GameWorldAuthoritative::MovePlayers(const float deltaTime)
	{
		for (std::optional<PlayerStateAuthoritative>& playerSlot : m_players)
		{
			if (!playerSlot.has_value())
			{
				continue;
			}

			PlayerStateAuthoritative& player = *playerSlot;

			glm::vec2 direction
			{ 
				static_cast<float>(player.CurrentInput.MoveX) / 127.0f, 
				static_cast<float>(player.CurrentInput.MoveY) / 127.0f
			};

			if (glm::dot(direction, direction) > 1.0f)
			{
				direction = glm::normalize(direction);
			}

			player.State.Velocity = direction * MAX_PLAYER_SPEED;
			player.State.Transform.Position += player.State.Velocity * deltaTime;

			player.CurrentInput.ActionsDown = 0;
			player.CurrentInput.ActionsUp = 0;
		}
	}

	bool GameWorldAuthoritative::AddPlayer(ClientID id)
	{
		if (id < 0 || static_cast<std::size_t>(id) >= m_players.size())
		{
			return false;
		}

		auto& playerSlot = m_players[static_cast<std::size_t>(id)];

		if (playerSlot.has_value())
		{
			return false;
		}

		playerSlot.emplace();
		playerSlot->State.ID = id;

		return true;
	}

	bool GameWorldAuthoritative::RemovePlayer(ClientID id)
	{
		if (id < 0 || static_cast<std::size_t>(id) >= m_players.size())
		{
			return false;
		}

		auto& playerSlot = m_players[static_cast<std::size_t>(id)];

		if (!playerSlot.has_value())
		{
			return false;
		}

		playerSlot.reset();
		return true;
	}

	void GameWorldAuthoritative::BroadcastPlayerJoinedEvents()
	{
		for (const auto& playerSlot : m_players)
		{
			if (!playerSlot.has_value())
			{
				continue;
			}

			const PlayerState& player = playerSlot->State;

			PlayerJoinedEvent event{};
			event.ID = player.ID;
			event.PositionX = player.Transform.Position.x;
			event.PositionY = player.Transform.Position.y;

			m_outgoingGameEvents.push(GameEvent{ event });
		}
	}

	void GameWorldAuthoritative::BroadcastPlayerDisconnectEvent(ClientID id)
	{
		PlayerDisconnectEvent disconnectEvent{};
		disconnectEvent.ID = id;

		m_outgoingGameEvents.push(GameEvent{ disconnectEvent });
	}

	void GameWorldAuthoritative::GeneratePlayerStateGameEvents()
	{
		for (const std::optional<PlayerStateAuthoritative>& playerSlot : m_players)
		{
			if (!playerSlot.has_value())
			{
				continue;
			}

			const PlayerState& player = playerSlot->State;

			PlayerStateEvent event{};
				event.ID = playerSlot->State.ID;
				event.ServerTick = m_serverTick;
				event.PositionX = player.Transform.Position.x;
				event.PositionY = player.Transform.Position.y;

			m_outgoingGameEvents.push(GameEvent{event});
		}
	}


} // namespace pldn