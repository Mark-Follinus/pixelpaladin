#pragma once

// Standard library includes
#include <string>

// Engine includes
#include "../engine/utilities/primitive_types.hpp"

// Forward Declarations
struct GLFWwindow;
struct GLFWmonitor;

class Device 
{
public:
    bool ShouldClose();
    void BeginFrame();
    void EndFrame();

    GLFWwindow* GetWindow() { return m_window; }
    GLFWmonitor* GetMonitor() { return m_monitor; };
    i32 GetWidth() const { return m_width; }
    i32 GetHeight() const { return m_height; }

    void SetWidth(i32 width) { m_width = width; }
    void SetHeight(i32 height) { m_height = height; }
    void SetWindowName(const std::string& name);

private:
	Device();
	~Device();

    static void ErrorCallback(i32 error, const char* description);

	void Update();

private:
	friend class Engine;

	GLFWwindow* m_window = nullptr;
	GLFWmonitor* m_monitor = nullptr;
	i32 m_width = -1;
	i32 m_height = -1;
    i32 m_maxWidth = -1;
    i32 m_maxHeight = -1;
};
