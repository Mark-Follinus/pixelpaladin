#pragma once

// Engine includes
#include "render/opengl/shaders/shader.hpp"

// Engine includes
#include "render/opengl/camera/camera_2d.hpp"

namespace pldn
{
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

	private:

		void LazyTempSetup();


		void DrawPlayers();

	private:

		bool m_initialized = false;

		Shader m_shader = {};
		unsigned int m_quadVBO = 0;
		unsigned int m_quadVAO = 0;

		const float m_quad[12] =
		{
			-0.5f, -0.5f, 0.0f,  // lower-left corner  
			 0.5f, -0.5f, 0.0f,  // lower-right corner
			 0.5f,  0.5f, 0.0f,  // upper-right corner
			-0.5f,  0.5f, 0.0f   // upper-left corner
		};

	};


}
