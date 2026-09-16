#include "renderer_2d_opengl.hpp"

// GLFW & GLAD libraries
#include <glad/glad.h>

// GLM libraries
#include "glm/glm.hpp"
#include <glm/ext/matrix_transform.hpp>

// Engine includes
#include "render/opengl/textures/texture_2d.hpp"

#include "render/opengl/data/sprite_2d.hpp"
#include "game/data/world_transform_2d.hpp"

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

	void Renderer2D::StartScene(const Camera2D& camera)
	{
		m_shader.Bind();
		m_shader.SetMatrix4("uProjection", camera.Projection());
		m_shader.SetMatrix4("uView", camera.View());
		m_shader.SetInt("uTexture", 0);
	}

	void Renderer2D::DrawScene()
	{
		DrawQuad(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::radians(45.0f));
	}

	void Renderer2D::EndScene()
	{
		m_shader.Unbind();
	}

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

	void Renderer2D::Shutdown()
	{

	}

	bool Renderer2D::IsInitialized() const
	{
		return m_initialized;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation)
	{
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, glm::vec3(position, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale, 1.0f));

		m_shader.SetMatrix4("uModel", model);

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);
	}

	void Renderer2D::DrawSprite(const Texture2D& texture, const SpriteTransform& spriteTransform, const WorldTransform2D& transform)
	{
		glm::mat4 model{ 1.0f };
		model = glm::translate(model, glm::vec3(transform.Position, 0.0f));
		model = glm::rotate(model, glm::radians(transform.Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(spriteTransform.PositionOffset, 0.0f));
		model = glm::rotate(model, glm::radians(spriteTransform.RotationOffset), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(spriteTransform.Scale, 1.0f));

		m_shader.SetMatrix4("uModel", model);

		const glm::vec2 uvScale // Calculate the size of one atlas cell.
		{
			1.0f / static_cast<float>(spriteTransform.AtlasColumns),
			1.0f / static_cast<float>(spriteTransform.AtlasRows)
		};

		const glm::vec2 uvOffset // Select the requested atlas cell.
		{
			static_cast<float>(spriteTransform.AtlasColumn) * uvScale.x,
			1.0f - static_cast<float>(spriteTransform.AtlasRow + 1) * uvScale.y
		};
		
		texture.Bind(0); // Send per-sprite state to OpenGL.
		m_shader.SetMatrix4("uModel", model);
		m_shader.SetVector2("uUVOffset", uvOffset);
		m_shader.SetVector2("uUVScale", uvScale);

		glBindVertexArray(m_quadVAO);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Renderer2D::DrawPlayers()
	{

	}
}
