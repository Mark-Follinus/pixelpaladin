#pragma once

// Forward declarations
class Device;
class Networking;
class Input;
class Renderer;

class Engine 
{
public:
	Engine();
	~Engine();

	void Run();

private:
	void Initialize();
	void Shutdown();

private:

	Device* m_device = nullptr;
	Networking* m_networking = nullptr;
	Input* m_input = nullptr;
	Renderer* m_renderer = nullptr;
};