#pragma once

namespace pldn 
{
    struct PacketHeader
    {
        enum class PacketType : unsigned short
        {
            Invalid = 0,
            ClientInput = 1,
            Disconnect = 2
        };

        PacketType type = PacketType::Invalid;
        unsigned short payloadSize = 0;
    };

    struct PacketBody
    {
        float FloatA = 0.0f, FloatB = 0.0f, FloatC = 0.0f, FloatD = 0.0f;
    };

    struct ClientInput
    {

    };
}