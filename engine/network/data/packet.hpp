#pragma once

#include "primitive_types.hpp"

struct PacketHeader
{
    enum class PacketType : u16
    {
        Invalid = 0,
        Test = 1,
        Disconnect = 2
    };

    PacketType type = PacketType::Invalid;
    u16 payloadSize = 0;
};

struct PacketBody
{
    u32 IntegerA = 0, IntegerB = 0;
    f32 FloatA = 0.0f, FloatB = 0.0f;
};