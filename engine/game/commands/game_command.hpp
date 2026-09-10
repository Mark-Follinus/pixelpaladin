#pragma once

// Standard Library Includes
#include <cstdint>

// GLM includes
#include <glm/glm.hpp>

namespace pldn
{
	enum class GameCommandType : uint8_t
	{
		Invalid,
		PlayerState
	};

	struct GameCommand
	{
		GameCommandType Type = GameCommandType::Invalid;

		uint8_t PlayerID = 0;
		uint32_t ServerTick = 0;

		float PositionX = 0.0f;
		float PositionY = 0.0f;
	};

	struct ClientGameCommand
	{
		int8_t ClientID = -1;
		GameCommand Command = {};
	};

	// -------------------------------------------------------------------------
	struct PlayerDataMovement
	{
		int32_t ClientID = -1;
		glm::vec2 Position = glm::vec2(0.0f);
		glm::vec2 Velocity = glm::vec2(0.0f);
	};

} // namespace pldn



