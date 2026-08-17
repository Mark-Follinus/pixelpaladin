#include "engine.hpp"

// Standard library includes
#include <cassert>
#include <chrono>

// Includes for forward declarations
#include "device/device.hpp"
#include "render/render.h"

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
		const f32 deltaTime = std::chrono::duration<f32>(currentTime - startTime).count();
		std::printf("FPS: %f", deltaTime);

		// m_input->Update();

		m_device->BeginFrame();
			m_renderer->Draw();
		m_device->EndFrame();

		m_device->Update();

		startTime = currentTime;
	}
}

// ------------------------
// Private Methods
// ------------------------

void Engine::Initialize()
{
	m_device = new Device();
	m_renderer = new Renderer();
}

void Engine::Shutdown()
{
	// delete m_input;
	// delete m_renderer;
	delete m_device;
	m_device = nullptr;
}
