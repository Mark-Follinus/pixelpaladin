#pragma once

// Standard Library Includes
#include <cstdint>

// GLM includes
#include <glm/glm.hpp>

namespace pldn
{
	struct PlayerDataMovement
	{
		int32_t ClientID = -1;
		glm::vec2 Position = glm::vec2(0.0f);
		glm::vec2 Velocity = glm::vec2(0.0f);
	};

	struct PlayerDataAction
	{

	};

	struct PlayerDataGameState
	{

	};

} // namespace pldn



