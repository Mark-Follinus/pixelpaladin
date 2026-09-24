#pragma once

// Standard library includes
#include <cstdint>

// GLM includes
#include "glm/glm.hpp"

namespace pldn
{
    using TextureID = unsigned int;

    struct Sprite2D
    {
        glm::vec2 PositionOffset{ 0.0f, 0.0f };
        glm::vec2 Scale{ 1.0f, 1.0f };
        float RotationOffset = 0.0f;

        unsigned int AtlasColumn = 0;
        unsigned int AtlasRow = 0;

        unsigned int AtlasColumns = 1;
        unsigned int AtlasRows = 1;
    };
}