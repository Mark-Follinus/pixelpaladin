#pragma once

// GLM includes
#include "glm/glm.hpp"

struct WorldTransform2D // Actual gameplay position and rotation.
{
    glm::vec2 Position{ 0.0f, 0.0f };
    float Rotation = 0.0f;
};