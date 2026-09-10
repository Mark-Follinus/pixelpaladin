#pragma once

// Standard library includes
#include <cstdint>
#include <string>

namespace pldn
{
	struct NetworkConfig
	{
		uint16_t ServerPort = 8080;
		std::string IPAddress = "127.0.0.1";
	};
}

