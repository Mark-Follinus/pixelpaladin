#pragma once

#include "./game/events/game_event.hpp"

namespace pldn
{
	class Device;
	class Networking;
	class CurrentInput;
	class Renderer;

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
		Networking* m_networking = nullptr;
		CurrentInput* m_input = nullptr;
		Renderer * m_renderer = nullptr;
	};

} // namespace pldn