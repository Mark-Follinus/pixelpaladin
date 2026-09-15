#pragma once

// Engine includes
#include "renderers/2d/renderer_2d_opengl.hpp"

namespace pldn 
{

	class RenderManager
	{
	public:
		void Draw();

	private:
		RenderManager();
		~RenderManager();

		void RenderBeginFrame();
		void RenderEndFrame();

	private:
		friend class Engine;
		friend class ClientEngine;

		Renderer2D m_renderer2D;
	};
}