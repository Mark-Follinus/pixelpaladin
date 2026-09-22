#pragma once

// Standard library includes
#include <cstdint>
#include <variant>

// Engine includes
#include "network/data/network_types.hpp"
#include "player_events.hpp"

namespace pldn
{
	enum class GameEventType : std::uint8_t
	{
		Invalid = 0,
		PlayerJoined = 1,
		PlayerLeft = 2,
		PlayerState = 3
	};

	using GameEvent = std::variant
	<
		std::monostate, 
		
		PlayerJoinedEvent, 
		PlayerLeftEvent, 
		PlayerStateEvent
	>;


} // namespace pldn



