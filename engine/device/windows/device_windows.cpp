#include "device_windows.hpp"

// Windows includes
#include <Windows.h>

// Standard Libraries
#include <cassert>

// GLFW & GLAD libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ------------------------
// Local Functions
// ------------------------
static void ErrorCallback(i32 error, const char* description) 
{ 
    fputs(description, stderr); 
}

// ------------------------
// Public Methods
// ------------------------

bool Device::ShouldClose()
{
	return glfwWindowShouldClose(m_window);
}

void Device::BeginFrame()
{

}

void Device::EndFrame()
{

}

void Device::SetWindowName(const std::string& name)
{
	glfwSetWindowTitle(m_window, name.c_str());
}

// ------------------------
// Private Constructors
// ------------------------ 

Device::Device()
{
    if (!glfwInit())
    {
        assert(false);
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE); // Debugging brev
    //  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    m_monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
    m_maxWidth = mode->width;
    m_maxHeight = mode->height;

    m_width = m_maxWidth;
    m_height = m_maxHeight;

    m_window = glfwCreateWindow(m_width, m_height, "Pixel Paladin", nullptr, nullptr);

    if (m_window == nullptr)
    {
        std::printf("Failed to create window.");
        glfwTerminate();
        assert(false);
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::printf("Failed to load GLAD.");
        glfwTerminate();
        assert(false);
        exit(EXIT_FAILURE);
    }
}

Device::~Device()
{
	glfwTerminate();
}

// ------------------------ 
// Private Methods
// ------------------------ 

void Device::Update()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}