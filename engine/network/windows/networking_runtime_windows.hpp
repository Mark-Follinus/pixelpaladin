#pragma once

// Windows library includes
#include <WinSock2.h>

namespace pldn
{

	class NetworkRuntime
	{
	public:
		bool IsInitialized() const noexcept { return m_initialized; }

	private:

		NetworkRuntime() = default;
		~NetworkRuntime() noexcept;

		NetworkRuntime(const NetworkRuntime&) = delete;            // Copy construction: forbidden
		NetworkRuntime& operator=(const NetworkRuntime&) = delete; // Copy assignment: forbidden

		NetworkRuntime(NetworkRuntime&&) = delete;                 // Move construction: forbidden
		NetworkRuntime& operator=(NetworkRuntime&&) = delete;      // Move assignment: forbidden

		bool Initialize();
		void Shutdown() noexcept;

	private:
		friend class Engine;
		friend class ClientEngine;
		friend class ServerEngine;

		WSADATA m_wsaData{};
		bool m_initialized = false;
	};
}