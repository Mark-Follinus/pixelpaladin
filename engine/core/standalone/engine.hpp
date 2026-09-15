#pragma once

#include "./game/events/game_event.hpp"

namespace pldn
{
	class Device;
	//class Networking;
	class Input;
	class RenderManager;

	class Engine
	{
	public:
		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(Engine&&) = delete;

		void Run();

	private:
		void Initialize();
		void Shutdown();

	private:
		Device* m_device = nullptr;
		//Networking* m_networking = nullptr;
		Input* m_input = nullptr;
		RenderManager * m_renderer = nullptr;
	};

} // namespace pldn