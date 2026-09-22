#include "client_engine.hpp"

// Standard library includes
#include <cassert>
#include <cstdio>
#include <chrono>
#include <thread>
#include <algorithm>
#include <utility>

// Engine includes
#include "device/device.hpp"
#include "network/network.hpp"
#include "input/input.hpp"
#include "game/game.hpp"
#include "render/render.hpp"

// Compile time variables
constexpr double TICK_RATE_HZ = 60.0;
constexpr std::chrono::duration<double> TICK_INTERVAL{ 1.0 / TICK_RATE_HZ };

namespace pldn
{
// ------------------------
// Public Constructors
// ------------------------

	ClientEngine::~ClientEngine() 
	{
		Shutdown();
	}

// ------------------------
// Public Methods
// ------------------------

	void ClientEngine::Run()
	{
		if (!Initialize())
		{
			return;
		}

		HostGame();
		m_networkClient->ConnectToServer("127.0.0.1", 8080);

		double accumulatedTime = 0.0f;
		std::chrono::time_point startTime = std::chrono::steady_clock::now();

		while (!m_device->ShouldClose())
		{
			const std::chrono::time_point currentTime = std::chrono::steady_clock::now();
			const double deltaTime = std::min(std::chrono::duration<double>(currentTime - startTime).count(), 0.25);
			startTime = currentTime;
			accumulatedTime += deltaTime;

			/* START FRAME */
			m_device->BeginFrame();
			m_device->Update();

			while (accumulatedTime >= TICK_INTERVAL.count())
			{
				/* NETWORK RECEIVE */
				m_networkClient->Receive();

				/* QUEUE INCOMING COMMANDS */
				GameEvent event;
				while (m_networkClient->PollGameEvent(event))
				{
					m_gameWorld->QueueGameEvent(event);
				}

				/* GAME UPDATE */
				m_gameWorld->Update(static_cast<float>(TICK_INTERVAL.count()));

				/* QUEUE OUTGOING INPUT */
				const InputCommand inputRaw = m_input->Update();
				m_networkClient->QueueInputCommand(inputRaw);

				/* NETWORK SEND*/
				m_networkClient->Send();

				accumulatedTime -= TICK_INTERVAL.count();
			}

			m_renderer->UpdateAnimations(*m_gameWorld, deltaTime);

			/* DRAW */
			m_renderer->DrawScene(*m_gameWorld);

			/* END FRAME */
			m_device->EndFrame();
		}

		Shutdown();
	}

// ------------------------
// Private Methods
// ------------------------

	bool ClientEngine::Initialize()
	{
		m_device = new Device();

		m_networkRuntime = new NetworkRuntime();
		if (!m_networkRuntime->Initialize())
		{
			std::printf("Error: Network runtime failed to initialize.\n");

			Shutdown();
			return false;
		}

		m_networkClient = new NetworkClient();
		m_input = new Input(m_device);
		m_gameWorld = new GameWorldReplicated();
		m_renderer = new RenderManager();
		m_renderer->Resize(static_cast<const unsigned int>(m_device->GetWidth()), static_cast<const unsigned int>(m_device->GetHeight()));

		return true;
	}

	void ClientEngine::Shutdown()
	{
		delete m_renderer;
		m_renderer = nullptr;

		delete m_gameWorld;
		m_gameWorld = nullptr;

		delete m_input;
		m_input = nullptr;

		delete m_networkClient;
		m_networkClient = nullptr;

		delete m_networkRuntime;
		m_networkRuntime = nullptr;

		delete m_device;
		m_device = nullptr;
	}

	void ClientEngine::HostGame()
	{

	}
}