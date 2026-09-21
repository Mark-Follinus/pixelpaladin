#pragma once

// Standard library includes
#include <cstdint>
#include <span>

namespace pldn
{

	class ByteReader
	{
	public:

		explicit ByteReader(std::span<const std::byte> bytes);

		bool ReadUInt8(std::uint8_t& u8);
		bool ReadUInt16(std::uint16_t& u16);
		bool ReadUInt32(std::uint32_t& u32);
		bool ReadUInt64(std::uint64_t& u64);

		bool ReadInt8(std::int8_t& i8);
		bool ReadInt16(std::int16_t& i16);
		bool ReadInt32(std::int32_t& i32);
		bool ReadInt64(std::int64_t& i64);

		bool ReadFloat32(float& f32);

	private:
		std::span<std::byte> m_bytes = {};
		std::size_t m_offset = 0;
	};

}
