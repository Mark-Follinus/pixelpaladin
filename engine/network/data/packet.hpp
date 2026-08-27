#pragma once

// Engine includes
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
    f32 FloatA = 0.0f, FloatB = 0.0f, FloatC = 0.0f, FloatD = 0.0f;
};