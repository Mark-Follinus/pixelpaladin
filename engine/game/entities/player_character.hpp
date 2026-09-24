#pragma once

// Engine includes
#include "game/replicated/player_state.hpp"

namespace pldn
{

    struct PlayerCharacter
    {
        PlayerState State{};
    };
}