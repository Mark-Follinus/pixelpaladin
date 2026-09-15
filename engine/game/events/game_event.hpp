#pragma once

// Standard library includes
#include <cstdint>

// Engine includes
#include "network/data/network_types.hpp"

namespace pldn
{
	enum class GameEventType : int8_t
	{
		Invalid = 0,
		PlayerState = 1
	};

	struct GameEvent
	{
		GameEventType Type = GameEventType::Invalid;

		ClientID ID = INVALID_CLIENT_ID;
		uint32_t ServerTick = 0;

		float PositionX = 0.0f;
		float PositionY = 0.0f;
	};

} // namespace pldn



