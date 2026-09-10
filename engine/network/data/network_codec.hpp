#pragma once

// Standard library includes
#include <cstddef>
#include <cstring>
#include <type_traits>

// Engine includes
#include "packet.hpp"
#include "input/commands/input_command.hpp"
#include "game/commands/game_command.hpp"

// Compiler time variables
constexpr size_t INPUT_COMMAND_SIZE_IN_BYTES = sizeof(pldn::InputCommand);
constexpr size_t GAME_COMMAND_SIZE_IN_BYTES = sizeof(pldn::GameCommand);

namespace pldn
{
	static_assert(std::is_trivially_copyable_v<InputCommand>, "Input command is too complex to copy cleanly.");
    static_assert(sizeof(InputCommand) == 16, "InputCommand wire layout changed.");

	static_assert(std::is_trivially_copyable_v<GameCommand>, "Game command is too complex to copy cleanly.");
    static_assert(sizeof(GameCommand) == 16, "GameCommand wire layout changed.");

// ------------------------
// Input Command
// ------------------------ 

    inline Packet SerializeInputCommand(const InputCommand& command)
    {
        Packet packet{};
            packet.Type = PacketType::InputCommand;
            packet.Payload.resize(INPUT_COMMAND_SIZE_IN_BYTES);

        std::memcpy(packet.Payload.data(), &command, INPUT_COMMAND_SIZE_IN_BYTES);

        return packet;
    }

    inline bool DeserializeInputCommand(const Packet& packet, InputCommand& command)
    {
        if (packet.Type != PacketType::InputCommand || packet.Payload.size() != INPUT_COMMAND_SIZE_IN_BYTES)
        {
            return false;
        }

        std::memcpy(&command, packet.Payload.data(), INPUT_COMMAND_SIZE_IN_BYTES);

        return true;
    }

// ------------------------
// Game Command
// ------------------------ 

    inline Packet SerializeGameCommand(const GameCommand& command)
    {
        Packet packet{};
            packet.Type = PacketType::GameCommand;
            packet.Payload.resize(GAME_COMMAND_SIZE_IN_BYTES);

        std::memcpy(packet.Payload.data(), &command, GAME_COMMAND_SIZE_IN_BYTES);

        return packet;
    }

    inline bool DeserializeGameCommand(const Packet& packet, GameCommand& command)
    {
        if (packet.Type != PacketType::GameCommand || packet.Payload.size() != GAME_COMMAND_SIZE_IN_BYTES)
        {
            return false;
        }

        std::memcpy(&command, packet.Payload.data(), GAME_COMMAND_SIZE_IN_BYTES);

        return true;
    }
}