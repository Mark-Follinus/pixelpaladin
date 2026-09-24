#pragma once

// Standard library includes
#include <cstdint>

// Engine includes
#include "input/commands/input_command.hpp"
#include "network/data/network_types.hpp"
#include "game/components/world_transform_2d.hpp"
#include "render/animation/animator_2d.hpp"

// Engine includes
namespace pldn
{
	enum class PlayerDirection : std::uint8_t
	{
		Down = 0,
		Right = 1,
		Left = 2,
		Up = 3,
	};

	enum class PlayerLocomotion : std::uint8_t 
	{
		Idle = 0,
		Moving = 1
	};

	struct PlayerState
	{
        ClientID ID = INVALID_CLIENT_ID;
        std::uint32_t LastProcessedInputSequence = 0;

		WorldTransform2D Transform{};
		PlayerDirection Direction = PlayerDirection::Down;
		PlayerLocomotion Locomotion = PlayerLocomotion::Idle;
	};

} // namespace pldn