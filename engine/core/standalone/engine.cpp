#include "engine.hpp"

// Standard library includes
#include <cassert>
#include <chrono>

// Includes for forward declarations
#include "device/device.hpp"
#include "network/network.hpp"
#include "render/render.hpp"

// Input includes
#include "input/commands/input_command.hpp"
#include "input/input.hpp"

namespace pldn
{

// ------------------------
// Public Constructors
// ------------------------

	Engine::Engine() {}

	Engine::~Engine()
	{
		Shutdown();
	}

// ------------------------
// Public Methods
// ------------------------

	void Engine::Run()
	{
		Initialize();

		std::chrono::time_point startTime = std::chrono::high_resolution_clock::now();

		while (!m_device->ShouldClose())
		{
			const std::chrono::time_point currentTime = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - startTime).count();

			/* START FRAME */
			m_device->BeginFrame();
			m_device->Update();

			/* LOCAL INPUT */
			const InputCommand& inputRaw = m_input->Update();

			/* NETWORK UPDATE */
			//m_networking->Update(inputRaw);

			/* GAME UPDATE */

			/* DRAW */
			//m_renderer->DrawScene();

			/* END FRAME */
			m_device->EndFrame();

			startTime = currentTime;
		}
	}

// ------------------------
// Private Methods
// ------------------------

	void Engine::Initialize()
	{
		m_device = new Device();

		//Networking::NetworkConfig networkConfig = {};
		//m_networking = new Networking(Networking::NetworkRole::Client, networkConfig);

		m_input = new Input(m_device);

		m_renderer = new RenderManager();
	}

	void Engine::Shutdown()
	{
		delete m_renderer;
		m_renderer = nullptr;

		delete m_input;
		m_input = nullptr;

		//delete m_networking;
		//m_networking = nullptr;

		delete m_device;
		m_device = nullptr;
	}

} // namespace pldn
