#pragma once

// Standard includes
#include <cstdint>

// GLM includes
#include "glm/glm.hpp"

// Engine includes
#include "input/commands/input_command.hpp"
#include "network/data/network_types.hpp"
#include "game/components/world_transform_2d.hpp"

// Engine includes
namespace pldn
{

	struct PlayerState
	{
        ClientID ID = -1;
        glm::vec2 Velocity = {};
        WorldTransform2D Transform{};
        uint32_t LastProcessedInputSequence = 0;
	};

} // namespace pldn