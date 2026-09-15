#pragma once

// Engine includes
#include "render/opengl/shaders/shader.hpp"

namespace pldn
{

	class Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		void Initialize();
		void Draw();
		void Shutdown();

		bool IsInitialized() const;

	private:

		void LazyTempSetup();
		void LazyTempDrawQuad();

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
