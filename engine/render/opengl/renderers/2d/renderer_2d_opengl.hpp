#pragma once

// Engine includes
#include "render/opengl/shaders/shader.hpp"

// Engine includes
#include "render/camera/camera_2d.hpp"

namespace pldn
{
	// Forward declarations
	struct Sprite2D;
	struct WorldTransform2D;
	class Texture2D;

	class Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		void StartScene(const Camera2D& camera);
		void DrawScene();
		void EndScene();

		void Initialize();
		void Shutdown();

		bool IsInitialized() const;
		void DrawQuad(const glm::vec2& position, const glm::vec2& scale, const float rotation);
		void DrawSprite(const Texture2D& texture, const Sprite2D& spriteTransform, const WorldTransform2D& transform);

	private:

		void LazyTempSetup();
		void DrawPlayers();

	private:

		bool m_initialized = false;

		Shader m_shader = {};
		unsigned int m_quadVBO = 0;
		unsigned int m_quadVAO = 0;

		const float m_quad[20] =
		{
			// Vertex Positions      // Texture Coordinates
			- 0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // lower-left corner 
			  0.5f, -0.5f, 0.0f,     1.0f, 0.0f, // lower-right corner
			  0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // upper-right corner
			 -0.5f,  0.5f, 0.0f,     0.0f, 1.0f  // upper-left corner
		};

	};


}
