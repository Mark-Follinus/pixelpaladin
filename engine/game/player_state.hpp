#pragma once

// Standard includes
#include <cstdint>

// GLM includes
#include "glm/glm.hpp"

// Engine includes
namespace pldn
{
	struct PlayerState
	{
        int8_t ID = 0;

        glm::vec2 Position = {};
        glm::vec2 Velocity = {};

        uint32_t LastProcessedInputSequence = 0;
	};

} // namespace pldn