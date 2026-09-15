#pragma once

// Engine includes
#include "camera/camera_2d.hpp"
#include "renderers/2d/renderer_2d_opengl.hpp"

namespace pldn 
{
	// Forward declarations
	class GameWorldReplicated;

	class RenderManager
	{
	public:
		void DrawScene(const GameWorldReplicated& world);

		void Resize(const unsigned int width, const unsigned int height);

	private:
		RenderManager();
		~RenderManager();

		void RenderBeginFrame();
		void RenderEndFrame();

	private:
		friend class Engine;
		friend class ClientEngine;

		Camera2D m_worldCamera;
		Renderer2D m_renderer2D;
	};
}