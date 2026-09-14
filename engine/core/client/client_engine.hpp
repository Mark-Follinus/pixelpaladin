#pragma once

namespace pldn
{
	class Device;
	class NetworkRuntime;
	class NetworkClient;
	class CurrentInput;
	class GameWorldReplicated;
	class Renderer;

	class ClientEngine
	{
	public:
		ClientEngine() = default;
		~ClientEngine();

		ClientEngine(const ClientEngine&) = delete;
		ClientEngine& operator=(const ClientEngine&) = delete;
		ClientEngine(ClientEngine&&) = delete;
		ClientEngine& operator=(ClientEngine&&) = delete;

		void Run();

	private:
		bool Initialize();
		void Shutdown();
		void HostGame();

	private:
		Device*					m_device = nullptr;
		NetworkRuntime* m_networkRuntime = nullptr;
		NetworkClient*   m_networkClient = nullptr;
		CurrentInput*					 m_input = nullptr;
		GameWorldReplicated* m_gameWorld = nullptr;
		Renderer*			  m_renderer = nullptr;
	};

}