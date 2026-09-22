#pragma once

// Standard library includes
#include <bit>
#include <limits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

namespace pldn
{

	class ByteReader
	{
	public:

		explicit ByteReader(std::span<const std::byte> bytes) noexcept : m_bytes(bytes) {}

        template <std::unsigned_integral T>
            requires (!std::same_as<std::remove_cv_t<T>, bool>) // Remove 'const' and 'volatile' qualifiers, then make sure 'T' is not a bool. C++ treats bools as uints.
        bool ReadUInt(T& value)
        {
            if (RemainingBytes() < sizeof(T))
            {
                return false;
            }

            T result = 0;

            for (std::size_t i = 0; i < sizeof(T); ++i)
            {
                const auto byte = std::to_integer<std::uint8_t>(m_bytes[m_offset + i]);

                result = static_cast<T>((result << 8) | static_cast<T>(byte));
            }

            m_offset += sizeof(T);
            value = result;

            return true;
        }

        template <std::signed_integral T>
        bool ReadInt(T& value)
        {
            using UnsignedT = std::make_unsigned_t<T>;

            UnsignedT bits = 0;

            if (!ReadUInt(bits))
            {
                return false;
            }

            value = std::bit_cast<T>(bits);
            return true;
        }

        bool ReadFloat(float& value)
        {
            static_assert(sizeof(float) == sizeof(std::uint32_t));
            static_assert(std::numeric_limits<float>::is_iec559);

            std::uint32_t bits = 0;

            if (!ReadUInt(bits))
            {
                return false;
            }

            value = std::bit_cast<float>(bits);
            return true;
        }

        bool ReadDouble(double& value)
        {
            static_assert(sizeof(double) == sizeof(std::uint64_t));
            static_assert(std::numeric_limits<double>::is_iec559);

            std::uint64_t bits = 0;

            if (!ReadUInt(bits))
            {
                return false;
            }

            value = std::bit_cast<double>(bits);
            return true;
        }

        bool ReadBool(bool& value)
        {
            std::uint8_t encodedValue = 0;

            if (!ReadUInt(encodedValue))
            {
                return false;
            }

            if (encodedValue > 1)
            {
                return false;
            }

            value = encodedValue == 1;
            return true;
        }

        [[nodiscard]]
        std::size_t RemainingBytes() const noexcept
        {
            return m_bytes.size() - m_offset;
        }

        [[nodiscard]]
        bool IsAtEnd() const noexcept
        {
            return m_offset == m_bytes.size();
        }

	private:
		std::span<const std::byte> m_bytes = {};
		std::size_t m_offset = 0;
	};

}
