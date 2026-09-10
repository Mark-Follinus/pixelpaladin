#pragma once

#include "./game/commands/game_command.hpp"

namespace pldn
{
	class Device;
	class Networking;
	class Input;
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
		Input* m_input = nullptr;
		Renderer * m_renderer = nullptr;

		PlayerDataMovement playerData;
	};

} // namespace pldn