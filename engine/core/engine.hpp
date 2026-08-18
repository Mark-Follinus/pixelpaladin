#pragma once

// Engine Includes
#include "primitive_types.hpp"

// Forward declarations
class Device;
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
	Input* m_input = nullptr;
	Renderer* m_renderer = nullptr;
};