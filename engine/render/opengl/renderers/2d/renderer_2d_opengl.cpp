#include "renderer_2d_opengl.hpp"

// GLFW & GLAD libraries
#include <glad/glad.h>

// GLM Libraries
#include "glm/glm.hpp"

namespace pldn
{

// ------------------------
// Public Constructors
// ------------------------
	Renderer2D::Renderer2D()
	{

	}

	Renderer2D::~Renderer2D()
	{

	}

// ------------------------
// Public Methods
// ------------------------

	void Renderer2D::Initialize()
	{
		LazyTempSetup();

		if (!m_shader.LazyTempLoad())
		{
			std::printf("Error: Renderer2D failed to initialize.\n");
			return;
		}

		m_initialized = true;
	}

	void Renderer2D::Draw()
	{
		m_shader.Bind();

		LazyTempDrawQuad();

		m_shader.Unbind();
	}

	void Renderer2D::Shutdown()
	{

	}

	bool Renderer2D::IsInitialized() const
	{
		return m_initialized;
	}

// ------------------------
// Private Methods
// ------------------------

	void Renderer2D::LazyTempSetup()
	{
		glGenVertexArrays(1, &m_quadVAO);

		glGenBuffers(1, &m_quadVBO);
		glBindVertexArray(m_quadVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_quad), m_quad, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Renderer2D::LazyTempDrawQuad()
	{ 
		glBindVertexArray(m_quadVAO);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindVertexArray(0);
	}

	void Renderer2D::DrawPlayers()
	{

	}
}
