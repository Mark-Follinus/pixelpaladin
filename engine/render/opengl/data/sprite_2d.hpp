#pragma once

// Standard library includes
#include <cstdint>

// GLM includes
#include "glm/glm.hpp"

namespace pldn
{
    using TextureID = std::uint32_t;

    struct Sprite2D // basically sprite transform.
    {
        TextureID Texture;

        glm::vec2 Size{ 1.0f, 1.0f };
        glm::vec2 Offset{ 0.0f, 0.0f };

        float RotationOffset = 0.0f;

        bool FlipX = false;
        bool FlipY = false;
    };
}