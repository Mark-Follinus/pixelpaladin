#pragma once

// Standard library includes
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <span>
#include <variant>
#include <vector>

// Engine includes
#include "input/commands/input_command.hpp"
#include "game/events/game_event.hpp"
#include "network/binary_stream/byte_writer.hpp"
#include "network/binary_stream/byte_reader.hpp"
#include "network_types.hpp"
#include "packet.hpp"

namespace pldn
{
    // Compiler time variables    
    constexpr std::size_t INPUT_COMMAND_WIRE_SIZE_IN_BYTES = (2 * sizeof(std::uint32_t)) 
                                                           + (3 * sizeof(std::uint16_t)) 
                                                           + (2 * sizeof(std::int8_t));
    static_assert(INPUT_COMMAND_WIRE_SIZE_IN_BYTES == 16);

    template<typename>
    inline constexpr bool ALWAYS_FALSE = false;

    // Forward declarations
    void WriteClientID(ByteWriter& writer, ClientID id);
    bool ReadClientID(ByteReader& reader, ClientID& id);

// ------------------------
// Input Command
// ------------------------ 

    inline Packet SerializeInputCommand(const InputCommand& command)
    {
        ByteWriter byteWriter;

        byteWriter.WriteUInt(command.SequenceNumber);
        byteWriter.WriteUInt(command.ClientTick);
        byteWriter.WriteUInt(command.ActionsDown);
        byteWriter.WriteUInt(command.ActionsHeld);
        byteWriter.WriteUInt(command.ActionsUp);
        byteWriter.WriteInt(command.MoveX);
        byteWriter.WriteInt(command.MoveY);

        Packet packet{};
        packet.Type = PacketType::InputCommand;
        packet.Payload = byteWriter.TakeBytes();

        return packet;
    }

    inline bool DeserializeInputCommand(const Packet& packet, InputCommand& command)
    {
        if (packet.Type != PacketType::InputCommand || packet.Payload.size() != INPUT_COMMAND_WIRE_SIZE_IN_BYTES)
        {
            return false;
        }

        ByteReader byteReader(std::span<const std::byte>(packet.Payload.data(), packet.Payload.size()));
        
        InputCommand readInputCommand{};

        if (!byteReader.ReadUInt(readInputCommand.SequenceNumber) 
        || !byteReader.ReadUInt(readInputCommand.ClientTick) 
        || !byteReader.ReadUInt(readInputCommand.ActionsDown) 
        || !byteReader.ReadUInt(readInputCommand.ActionsHeld) 
        || !byteReader.ReadUInt(readInputCommand.ActionsUp) 
        || !byteReader.ReadInt(readInputCommand.MoveX) 
        || !byteReader.ReadInt(readInputCommand.MoveY)
        || !byteReader.IsAtEnd())
        {
            return false;
        }

        command = readInputCommand;
        return true;
    }

// ------------------------
// Game Event
// ------------------------ 

    inline Packet SerializeGameEvent(const GameEvent& event)
    {
        ByteWriter byteWriter;

        std::visit(
            [&byteWriter](const auto& concreteEvent)
            {
                using Event = std::remove_cvref_t<decltype(concreteEvent)>;

                if constexpr (std::same_as<Event, std::monostate>)
                {
                    byteWriter.WriteUInt(static_cast<std::uint8_t>(GameEventType::Invalid));
                }
                else if constexpr (std::same_as<Event, PlayerJoinedEvent>)
                {
                    byteWriter.WriteUInt(static_cast<std::uint8_t>(GameEventType::PlayerJoined));

                    WriteClientID(byteWriter, concreteEvent.ID);
                    byteWriter.WriteFloat(concreteEvent.PositionX);
                    byteWriter.WriteFloat(concreteEvent.PositionY);
                }
                else if constexpr (std::same_as<Event, PlayerLeftEvent>)
                {
                    byteWriter.WriteUInt(static_cast<std::uint8_t>(GameEventType::PlayerLeft));

                    WriteClientID(byteWriter, concreteEvent.ID);
                }
                else if constexpr (std::same_as<Event, PlayerStateEvent>)
                {
                    byteWriter.WriteUInt(static_cast<std::uint8_t>(GameEventType::PlayerState));

                    WriteClientID(byteWriter, concreteEvent.ID);
                    byteWriter.WriteUInt(concreteEvent.ServerTick);
                    byteWriter.WriteFloat(concreteEvent.PositionX);
                    byteWriter.WriteFloat(concreteEvent.PositionY);
                }
                else
                {
                    static_assert(ALWAYS_FALSE<Event>, "GameEvent alternative has no serializer.");
                }
            },
        event);

        Packet packet{};
        packet.Type = PacketType::GameEvent;
        packet.Payload = byteWriter.TakeBytes();

        return packet;
    }

    inline bool DeserializeGameEvent(const Packet& packet, GameEvent& event)
    {
        if (packet.Type != PacketType::GameEvent)
        {
            return false;
        }

        ByteReader byteReader(std::span<const std::byte>(packet.Payload.data(), packet.Payload.size()));

        std::uint8_t encodedType = 0;

        if (!byteReader.ReadUInt(encodedType))
        {
            return false;
        }

        GameEvent eventDecoded{};

        switch (static_cast<GameEventType>(encodedType))
        {
            case GameEventType::Invalid:
            {
                eventDecoded = std::monostate{};
                break;
            }
            case GameEventType::PlayerJoined:
            {
                PlayerJoinedEvent eventJoined{};

                if (!ReadClientID(byteReader, eventJoined.ID) 
                    || !byteReader.ReadFloat(eventJoined.PositionX) 
                    || !byteReader.ReadFloat(eventJoined.PositionY))
                {
                    return false;
                }

                eventDecoded = eventJoined;
                break;
            }
            case GameEventType::PlayerLeft:
            {
                PlayerLeftEvent eventLeft{};

                if (!ReadClientID(byteReader, eventLeft.ID))
                {
                    return false;
                }

                eventDecoded = eventLeft;
                break;
            }

            case GameEventType::PlayerState:
            {
                PlayerStateEvent eventPlayerState{};

                if (!ReadClientID(byteReader, eventPlayerState.ID) 
                    || !byteReader.ReadUInt(eventPlayerState.ServerTick) 
                    || !byteReader.ReadFloat(eventPlayerState.PositionX) 
                    || !byteReader.ReadFloat(eventPlayerState.PositionY))
                {
                    return false;
                }

                eventDecoded = eventPlayerState;
                break;
            }

            default:
            {
                // Unknown event type.
                return false;
            }
        }

        if (!byteReader.IsAtEnd())
        {
            // The event contained unexpected trailing data.
            return false;
        }

        event = std::move(eventDecoded);
        return true;
    }

// ------------------------
// Helper Functions
// ------------------------ 

    inline void WriteClientID(ByteWriter& writer, ClientID id)
    {
        writer.WriteInt(static_cast<std::int8_t>(id));
    }

    inline bool ReadClientID(ByteReader& reader, ClientID& id)
    {
        std::int8_t encodedID = 0;

        if (!reader.ReadInt(encodedID))
        {
            return false;
        }

        id = static_cast<ClientID>(encodedID);
        return true;
    }
}