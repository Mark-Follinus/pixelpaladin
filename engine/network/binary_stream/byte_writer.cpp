#include "byte_writer.hpp"

// Standard library includes
#include <span>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void ByteWriter::WriteUInt8(std::uint8_t u8) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &u8, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}
	
	void ByteWriter::WriteUInt16(std::uint16_t u16) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &u16, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteUInt32(std::uint32_t u32) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &u32, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteUInt64(std::uint64_t u64) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &u64, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteInt8(std::int8_t i8) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &i8, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteInt16(std::int16_t i16) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &i16, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteInt32(std::int32_t i32) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &i32, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteInt64(std::int64_t i64) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &i64, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	void ByteWriter::WriteFloat32(float f32) 
	{
		//std::span<const std::byte> bytes = std::as_bytes(std::span{ &f32, 1 });
		//m_bytes.insert(m_bytes.end(), m_bytes.begin(), m_bytes.end());
	}

	std::vector<std::byte> ByteWriter::Bytes() 
	{
		return m_bytes;
	}
}