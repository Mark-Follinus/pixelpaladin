#pragma once

// Standard library includes
#include <array>
#include <queue>
#include <optional>

// Engine includes
#include "game/constants/game_constants.hpp"
#include "game/events/game_event.hpp"
#include "game/replicated/player_state.hpp"

namespace pldn
{
	class GameWorldReplicated
	{

	public:
		void Update(const float /* deltaTime */);
		void QueueGameEvent(GameEvent command);
		const std::array<std::optional<PlayerState>, MAX_PLAYERS_ALLOWED>& GetPlayers() const { return m_players; }

	private:
		GameWorldReplicated() = default;
		~GameWorldReplicated();

		void ApplyPlayerStateEvent(const PlayerStateEvent& event);
		void ApplyPlayerJoinedEvent(const PlayerJoinedEvent& event);
		void ApplyPlayerDisconnectEvent(const PlayerDisconnectEvent& event);

	private:
		friend class Engine;
		friend class ClientEngine;

		std::array<std::optional<PlayerState>, MAX_PLAYERS_ALLOWED> m_players = {};

		std::queue<GameEvent> m_incomingGameEvents = {};
	};

} // namespace pldn
