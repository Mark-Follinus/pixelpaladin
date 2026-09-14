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
		ProcessInputCommands();

		// Continous game systems
		MovePlayers(deltaTime);

		// Discrete game systems (will manaully add events to the queue, if something happens)
		// ...

		// Continuous state replication is generated at the end of the tick. (Every tick send event)
		GenerateGameEvents();

		m_serverTick++;
	}

	void GameWorldAuthoritative::QueueInputCommand(ClientInputCommand command) 
	{
		m_incomingInputCommands.push(std::move(command));
	}

	bool GameWorldAuthoritative::PollGameEvent(ClientGameEvent& command)
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
				slot.emplace();
				slot->State.ID = clientID;
			}

			PlayerStateAuthoritative& player = *slot;
			const InputCommand& command = incoming.Command;

			if (command.SequenceNumber <= player.State.LastProcessedInputSequence)
			{
				continue;
			}

			player.State.LastProcessedInputSequence = command.SequenceNumber;
			player.CurrentInput = command;


			// Once attacks matter, inputs can be perserved like this in the case of multiple inputs being received in the same tick.
			//player.CurrentInput.ActionsDown |= command.ActionsDown;
			//player.CurrentInput.ActionsUp |= command.ActionsUp;

			//player.CurrentInput.ActionsHeld = command.ActionsHeld;
			//player.CurrentInput.MoveX = command.MoveX;
			//player.CurrentInput.MoveY = command.MoveY;
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
			player.State.Position += player.State.Velocity * deltaTime;

			player.CurrentInput.ActionsDown = 0;
			player.CurrentInput.ActionsUp = 0;
		}
	}

	void GameWorldAuthoritative::GenerateGameEvents()
	{
		for (const std::optional<PlayerStateAuthoritative>& playerSlot : m_players)
		{
			if (!playerSlot.has_value())
			{
				continue;
			}

			const PlayerState& player = playerSlot->State;

			ClientGameEvent event{};
			event.RecipientClientID = player.ID;
			event.Event.Type = GameEventType::PlayerState;
			event.Event.ServerTick = m_serverTick;
			event.Event.PositionX = player.Position.x;
			event.Event.PositionY = player.Position.y;

			m_outgoingGameEvents.push(std::move(event));
		}
	}


} // namespace pldn