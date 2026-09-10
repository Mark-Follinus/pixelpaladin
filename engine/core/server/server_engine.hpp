#pragma once

namespace pldn
{
	class NetworkRuntime;
	class NetworkServer;
	class GameWorldAuthoritative;

	class ServerEngine
	{
	public:
		ServerEngine() = default;
		~ServerEngine();

		ServerEngine(const ServerEngine&) = delete;
		ServerEngine& operator=(const ServerEngine&) = delete;
		ServerEngine(ServerEngine&&) = delete;
		ServerEngine& operator=(ServerEngine&&) = delete;

		void Run();

	private:
		bool Initialize();
		void Shutdown();

	private:

		NetworkRuntime*         m_networkRuntime = nullptr;
		NetworkServer*          m_networkServer = nullptr;
		GameWorldAuthoritative* m_gameWorld = nullptr;

		bool m_running = false;
	};

} // namespace pldn