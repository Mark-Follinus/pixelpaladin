#pragma once

// Standard library includes
#include <cstdint>
#include <vector>

namespace pldn
{

	class ByteWriter
	{
	public:

		void WriteUInt8(std::uint8_t u8);
		void WriteUInt16(std::uint16_t u16);
		void WriteUInt32(std::uint32_t u32);
		void WriteUInt64(std::uint64_t u64);

		void WriteInt8(std::int8_t i8);
		void WriteInt16(std::int16_t i16);
		void WriteInt32(std::int32_t i32);
		void WriteInt64(std::int64_t i64);

		void WriteFloat32(float value);

		std::vector<std::byte> Bytes();

	private:
		std::vector<std::byte> m_bytes = {};
	};

}