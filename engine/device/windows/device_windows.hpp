#pragma once

// Standard library includes
#include <string>

// Forward Declarations
struct GLFWwindow;
struct GLFWmonitor;

namespace pldn
{
    class Device 
    {
    public:
        bool ShouldClose();
        void BeginFrame();
        void EndFrame();

        GLFWwindow* GetWindow() { return m_window; }
        GLFWmonitor* GetMonitor() { return m_monitor; };
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        void SetWidth(int width) { m_width = width; }
        void SetHeight(int height) { m_height = height; }
        void SetWindowName(const std::string& name);

    private:
	    Device();
	    ~Device();

        static void ErrorCallback(int error, const char* description);

	    void Update();

    private:
	    friend class Engine;
        friend class ClientEngine;
        friend class ServerEngine;

	    GLFWwindow* m_window = nullptr;
	    GLFWmonitor* m_monitor = nullptr;
	    int m_width = -1;
	    int m_height = -1;
        int m_maxWidth = -1;
        int m_maxHeight = -1;
    };

} // namespace pldn