#pragma once

// Standard library includes
#include <cstddef>
#include <cstring>
#include <type_traits>

// Engine includes
#include "input/commands/input_command.hpp"
#include "game/events/game_event.hpp"
#include "network_types.hpp"
#include "packet.hpp"

namespace pldn
{
    // Compiler time variables
    constexpr size_t INPUT_COMMAND_SIZE_IN_BYTES = sizeof(pldn::InputCommand);
    constexpr size_t GAME_EVENT_SIZE_IN_BYTES = sizeof(pldn::GameEvent);

	static_assert(std::is_trivially_copyable_v<InputCommand>, "Input command is too complex to copy cleanly.");
    static_assert(sizeof(InputCommand) == 16, "InputCommand wire layout changed.");

	static_assert(std::is_trivially_copyable_v<GameEvent>, "GameEvent is too complex to copy cleanly.");
    static_assert(sizeof(GameEvent) == 16, "GameEvent wire layout changed.");

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

        /*
            ByteWriter writer;

            writer.WriteU32(command.SequenceNumber);
            writer.WriteU32(command.ClientTick);
            writer.WriteU16(command.ActionsDown);
            writer.WriteU16(command.ActionsHeld);
            writer.WriteU16(command.ActionsUp);
            writer.WriteI8(command.MoveX);
            writer.WriteI8(command.MoveY);

            Packet packet{};
            packet.Type = PacketType::InputCommand;
            packet.Payload = writer.TakeBytes();

            return packet;
        */

    }

    inline bool DeserializeInputCommand(const Packet& packet, InputCommand& command)
    {
        if (packet.Type != PacketType::InputCommand || packet.Payload.size() != INPUT_COMMAND_SIZE_IN_BYTES)
        {
            return false;
        }

        std::memcpy(&command, packet.Payload.data(), INPUT_COMMAND_SIZE_IN_BYTES);

        return true;

        /*
            if (packet.Type != PacketType::InputCommand)
            {
                return false;
            }

            ByteReader reader(packet.Payload);

            if (!reader.ReadU32(command.SequenceNumber) || !reader.ReadU32(command.ClientTick) || !reader.ReadU16(command.ActionsDown) || !reader.ReadU16(command.ActionsHeld) || !reader.ReadU16(command.ActionsUp) || !reader.ReadI8(command.MoveX) || !reader.ReadI8(command.MoveY))
            {
                return false;
            }

            return reader.IsFinished();
        */
    }

// ------------------------
// Game Event
// ------------------------ 

    inline Packet SerializeGameEvent(const GameEvent& event)
    {
        Packet packet{};
            packet.Type = PacketType::GameEvent;
            packet.Payload.resize(GAME_EVENT_SIZE_IN_BYTES);

        std::memcpy(packet.Payload.data(), &event, GAME_EVENT_SIZE_IN_BYTES);

        return packet;
    }

    inline bool DeserializeGameEvent(const Packet& packet, GameEvent& event)
    {
        if (packet.Type != PacketType::GameEvent || packet.Payload.size() != GAME_EVENT_SIZE_IN_BYTES)
        {
            return false;
        }

        std::memcpy(&event, packet.Payload.data(), GAME_EVENT_SIZE_IN_BYTES);

        return true;
    }
}