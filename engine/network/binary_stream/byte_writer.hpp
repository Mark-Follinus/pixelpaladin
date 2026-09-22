#pragma once

// Standard library includes
#include <bit>
#include <climits>
#include <limits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace pldn
{

	class ByteWriter
	{
	public:

		template <std::unsigned_integral T>
			requires (!std::same_as<std::remove_cv_t<T>, bool>) // Remove 'const' and 'volatile' qualifiers, then make sure 'T' is not a bool. C++ treats bools as uints.
		void WriteUInt(T value)
		{
			static_assert(CHAR_BIT == 8);

			// Big-endian ordering. Stolen from the internet.
			for (std::size_t i = sizeof(T); i > 0; i--) 
			{
				const std::size_t shift = (i - 1) * 8;
				const auto byte = static_cast<std::uint8_t>((value >> shift) & static_cast<T>(0xFF));
				m_bytes.push_back(static_cast<std::byte>(byte));
			}
		}

		template <std::signed_integral T>
		void WriteInt(T value)
		{
			// Conversion to unsigned preserves the integer's modulo bit pattern.
			using UnsignedT = std::make_unsigned_t<T>;
			WriteUInt(static_cast<UnsignedT>(value)); 
		}

		void WriteBool(bool value)
		{
			WriteUInt(static_cast<std::uint8_t>(value ? 1 : 0));
		}

		void WriteFloat(float value)
		{
			static_assert(sizeof(float) == sizeof(std::uint32_t));
			static_assert(std::numeric_limits<float>::is_iec559);

			WriteUInt(std::bit_cast<std::uint32_t>(value));
		}

		void WriteDouble(double value)
		{
			static_assert(sizeof(double) == sizeof(std::uint64_t));
			static_assert(std::numeric_limits<double>::is_iec559);

			WriteUInt(std::bit_cast<std::uint64_t>(value));
		}

		[[nodiscard]]
		const std::vector<std::byte>& Bytes() const noexcept
		{
			return m_bytes;
		}

		[[nodiscard]]
		std::vector<std::byte> TakeBytes() noexcept
		{
			return std::move(m_bytes);
		}

	private:
		std::vector<std::byte> m_bytes = {};
	};

}