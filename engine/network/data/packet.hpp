#pragma once

// Standard library includes
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pldn
{
    enum class PacketType : uint16_t
    {
        Invalid      = 0,
        InputCommand = 1,
        GameCommand  = 2
    };

    struct PacketHeader
    {
        PacketType Type = PacketType::Invalid;
        std::uint16_t PayloadSize = 0;
    };
    
    static_assert(sizeof(PacketHeader) == 4);

    struct Packet
    {
        PacketType Type = PacketType::Invalid;
        std::vector<std::byte> Payload;
    };
}