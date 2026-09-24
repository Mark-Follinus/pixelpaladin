#pragma once

// Engine includes
#include "render/animation/animator_2d.hpp"
#include "render/sprites/sprite_2d.hpp"

namespace pldn
{
    struct PlayerVisual
    {
        Sprite2D Sprite{};
        Animator2D Animator{};
    };
}