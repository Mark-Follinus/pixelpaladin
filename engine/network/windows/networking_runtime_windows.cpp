#include "networking_runtime_windows.hpp"

// Standard library includes
#include <cstdint>
#include <cstdio>

// Compile time variables
constexpr std::uint8_t WINSOCK_VERSION_MAJOR = 2;
constexpr std::uint8_t WINSOCK_VERSION_MINOR = 2;

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

// ------------------------
// Private Constructors
// ------------------------ 

	NetworkRuntime::~NetworkRuntime()
	{
		Shutdown();
	}

// ------------------------
// Private Methods
// ------------------------

	bool NetworkRuntime::Initialize()
	{
		if (m_initialized)
		{
			return true;
		}

		constexpr WORD wVersionRequested = MAKEWORD(WINSOCK_VERSION_MAJOR, WINSOCK_VERSION_MINOR);
		const int wsaError = WSAStartup(wVersionRequested, &m_wsaData);
		if (wsaError != 0)
		{
			std::printf("NETWORK: WSA startup failed: %i\n", wsaError);
			return false;
		}

		if (m_wsaData.wVersion != wVersionRequested)
		{
			std::printf("NETWORK: WinSock 2.2 is unavailable.\n");

			WSACleanup();
			m_wsaData = {};

			return false;
		}

		std::printf("NETWORK: WSA initialized.\n");

		m_initialized = true;
		return true;

	}

	void NetworkRuntime::Shutdown() noexcept
	{
		if (!m_initialized)
		{
			return;
		}

		if (WSACleanup() == SOCKET_ERROR)
		{
			const int error = WSAGetLastError();
			std::printf("Network: WSACleanup failed: %i\n", error);
		}
		else
		{
			std::printf("Network: WinSock shut down.\n");
		}

		m_wsaData = {};
		m_initialized = false;
	}
}