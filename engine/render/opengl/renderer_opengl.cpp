#include "renderer_opengl.hpp"

// Windows includes
//#include <Windows.h>

// Standard Libraries
//#include <cassert>

// GLFW & GLAD libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Libraries
#include "glm/glm.hpp"

// ------------------------
// Public Methods
// ------------------------

void Renderer::Draw()
{
	RenderStart();
	RenderQuad();
	RenderEnd();
}

// ------------------------
// Private Methods
// ------------------------

void Renderer::RenderStart()
{
	glClearColor(0.471f, 0.741f, 0.596f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::RenderQuad()
{

}

void Renderer::RenderEnd()
{
	
}


Renderer::Renderer() {}

Renderer::~Renderer() {}
