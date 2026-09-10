#include "server_engine.hpp"

// Standard library includes
#include <cstdio>
#include <chrono>
#include <thread>

// Engine includes
#include "network/network.hpp"
#include "game/game.hpp"

// Compile time variables
constexpr double TICK_RATE_HZ = 60.0;
constexpr std::chrono::duration<double> TICK_INTERVAL { 1.0 / TICK_RATE_HZ };

namespace pldn
{
// ------------------------
// Public Constructors
// ------------------------

	ServerEngine::~ServerEngine()
	{
		Shutdown();
	}

// ------------------------
// Public Methods
// ------------------------

	void ServerEngine::Run()
	{
		if (!Initialize())
		{
			return;
		}

		m_running = true;

		using Clock = std::chrono::steady_clock;
		Clock::time_point nextTick = Clock::now();

		while (m_running)
		{
			nextTick += std::chrono::duration_cast<Clock::duration>(TICK_INTERVAL);

			/* NETWORK RECEIVE */
			m_networkServer->Receive();

			/* QUEUE INCOMING INPUTS */
			int8_t clientID = -1;
			InputCommand inputCommand{};
			while (m_networkServer->PollInputCommand(clientID, inputCommand))
			{
				m_gameWorld->QueueInputCommand({clientID, std::move(inputCommand)});
			}

			/* UPDATE WORLD */
			m_gameWorld->Update(static_cast<float>(TICK_INTERVAL.count()));

			/* QUEUE OUTGOING GAME COMMANDS */
			ClientGameCommand clientCommand{};
			while (m_gameWorld->PollGameCommand(clientCommand))
			{
				m_networkServer->QueueGameCommand(clientCommand.ClientID, std::move(clientCommand.Command));
			}

			/* NETWORK SEND */
			m_networkServer->Send();

			std::this_thread::sleep_until(nextTick);
		}

		m_running = false;
		Shutdown();
	}

// ------------------------
// Private Methods
// ------------------------

	bool ServerEngine::Initialize()
	{
		m_networkRuntime = new NetworkRuntime();
		if (!m_networkRuntime->Initialize())
		{
			std::printf("Error: Network runtime failed to initialize.\n");

			Shutdown();
			return false;
		}

		m_networkServer = new NetworkServer("127.0.0.1", 8080);
		if (!m_networkServer->Start())
		{
			std::printf("ERROR: Network server failed to start.\n");

			Shutdown();
			return false;
		}

		m_gameWorld = new GameWorldAuthoritative();

		return true;
	}

	void ServerEngine::Shutdown()
	{
		delete m_gameWorld;
		m_gameWorld = nullptr;

		delete m_networkServer;
		m_networkServer = nullptr;

		delete m_networkRuntime;
		m_networkRuntime = nullptr;
	}

} // namespace pldn