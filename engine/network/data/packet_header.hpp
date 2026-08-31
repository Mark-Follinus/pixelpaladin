#pragma once

namespace pldn 
{
    enum class PacketType : unsigned short
    {
        Invalid = 0,
        ClientInputCommand = 1,
        ClientChat = 2,
        ClientDisconnect = 3
    };

    struct PacketHeader
    {
        PacketType type = PacketType::Invalid;
        unsigned short payloadSize = 0;
    };

    struct PacketBodyTest
    {
        float FloatA = 0.0f, FloatB = 0.0f, FloatC = 0.0f, FloatD = 0.0f;
    };
}