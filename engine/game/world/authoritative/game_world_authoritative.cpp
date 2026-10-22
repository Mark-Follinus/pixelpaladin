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

	bool GameWorldAuthoritative::PollGameEvent(GameEvent& event)
	{
		if (m_outgoingGameEvents.empty())
		{
			return false;
		}

		event = std::move(m_outgoingGameEvents.front());

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

			const std::size_t playerIndex = static_cast<std::size_t>(clientID);
			std::optional<PlayerCharacter>& playerSlot = m_players[playerIndex];

			if (!playerSlot.has_value())
			{
				continue;
			}

			PlayerCharacter& player = *playerSlot;
			const InputCommand& command = incoming.Command;

			if (command.SequenceNumber <= player.State.LastProcessedInputSequence)
			{
				continue;
			}

			player.State.LastProcessedInputSequence = command.SequenceNumber;
			m_currentInputs[playerIndex] = command;
		}
	}

	void GameWorldAuthoritative::MovePlayers(const float deltaTime)
	{
		for (std::size_t i = 0; i < m_players.size(); ++i)
		{
			if (!m_players[i].has_value())
			{
				continue;
			}

			PlayerCharacter& player = *m_players[i];
			const InputCommand& input = m_currentInputs[i];

			glm::vec2 movementInput
			{
				static_cast<float>(input.MoveX) / 127.0f,
				static_cast<float>(input.MoveY) / 127.0f
			};

			if (glm::dot(movementInput, movementInput) > 1.0f)
			{
				movementInput = glm::normalize(movementInput);
			}

			const bool isMoving = movementInput.x != 0.0f || movementInput.y != 0.0f;

			player.State.Locomotion = isMoving ? PlayerLocomotion::Moving : PlayerLocomotion::Idle;

			// Preserve the last direction while idle.
			if (isMoving)
			{
				if (std::abs(movementInput.x) > std::abs(movementInput.y))
				{
					player.State.Direction = movementInput.x > 0.0f ? PlayerDirection::Right : PlayerDirection::Left;
				}
				else
				{
					player.State.Direction = movementInput.y > 0.0f ? PlayerDirection::Up : PlayerDirection::Down;
				}
			}

			player.State.Transform.Position += movementInput * MAX_PLAYER_SPEED * deltaTime;
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
		m_currentInputs[static_cast<std::size_t>(id)] = {};

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
		for (const std::optional<PlayerCharacter>& playerSlot : m_players)
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
				event.Direction = player.Direction;
				event.Locomotion = player.Locomotion;

			m_outgoingGameEvents.push(GameEvent{event});
		}
	}

} // namespace pldn