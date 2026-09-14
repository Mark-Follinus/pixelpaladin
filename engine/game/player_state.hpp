#pragma once

// Standard includes
#include <cstdint>

// GLM includes
#include "glm/glm.hpp"

// Engine includes
#include "input/commands/input_command.hpp"
#include "./network/data/network_types.hpp"

// Engine includes
namespace pldn
{
	struct PlayerState
	{
        ClientID ID = -1;

        glm::vec2 Position = {};
        glm::vec2 Velocity = {};

        uint32_t LastProcessedInputSequence = 0;
	};

} // namespace pldn