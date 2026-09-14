#pragma once

// Standard library includes
#include <cstdint>

// Engine includes
#include "./network/data/network_types.hpp"

namespace pldn
{
	enum class InputCommandTypeAction : uint16_t
	{
		None = 0,

		// ---

		Attack     = 1 << 0,
		Block      = 1 << 1,
		Dodge      = 1 << 2,
		Projectile = 1 << 3
	};

	struct InputCommand
	{
		// Incremented for every input command the client creates/sends.
		uint32_t SequenceNumber = 0;

		// Client simulation tick on which this input was captured.
		uint32_t ClientTick = 0;

		// Player actions.
		uint16_t ActionsDown = 0; // 0000'0000  0000'0000
		uint16_t ActionsHeld = 0; // 0000'0000  0000'0000 
		uint16_t ActionsUp   = 0; // 0000'0000  0000'0000

		// Player analog movement
		int8_t MoveX = 0; // 0000'0000
		int8_t MoveY = 0; // 0000'0000
	};

	struct ClientInputCommand
	{
		ClientID ClientID = INVALID_CLIENT_ID;
		InputCommand Command = {};
	};
}