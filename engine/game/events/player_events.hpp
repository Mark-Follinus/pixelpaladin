#pragma once

// Standard library includes
#include <cstdint>

// Engine includes
#include "network/data/network_types.hpp"

namespace pldn
{

    struct PlayerJoinedEvent
    {
        ClientID ID = INVALID_CLIENT_ID;

        float PositionX = 0.0f;
        float PositionY = 0.0f;
    };

    struct PlayerDisconnectEvent
    {
        ClientID ID = INVALID_CLIENT_ID;
    };

    struct PlayerStateEvent
    {
        ClientID ID = INVALID_CLIENT_ID;
        std::uint32_t ServerTick = 0;

        float PositionX = 0.0f;
        float PositionY = 0.0f;
    };

}