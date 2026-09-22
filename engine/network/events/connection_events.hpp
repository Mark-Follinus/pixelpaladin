#pragma once

// Standard library includes
#include <cstdint>

// Engine includes
#include "../data/network_types.hpp"

namespace pldn
{

    enum class ClientConnectionChangeEventType : std::uint8_t
    {
        Connected,
        Disconnected
    };

    // This is never serialized. It only travels: NetworkServer -> ServerEngine -> GameWorldAuthoritative.
    struct ClientConnectionChangeEvent
    {
        ClientConnectionChangeEventType Type = ClientConnectionChangeEventType::Disconnected;
        ClientID ID = INVALID_CLIENT_ID;
    };

}