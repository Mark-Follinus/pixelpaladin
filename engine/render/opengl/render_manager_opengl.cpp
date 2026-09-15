#include "render_manager_opengl.hpp"

// GLFW & GLAD libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM Libraries
#include "glm/glm.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void RenderManager::Draw()
	{
		/* BEGIN FRAME */
		RenderBeginFrame();

		/* DRAW WORLD*/

		if (m_renderer2D.IsInitialized())
		{
			m_renderer2D.Draw();
		}

		//RenderTerrain()
		//RenderEnemies()
		//RenderItems()

		/* IMGUI */
		// imgui layer begin Frame
		// -- Draw network debug
		// -- Draw world debug
		// imgui layer end frame

		/* END FRAME */
		RenderEndFrame();
	}

// ------------------------
// Private Constructors
// ------------------------

	RenderManager::RenderManager() 
	{
		m_renderer2D.Initialize();


	}

	RenderManager::~RenderManager() {}

// ------------------------
// Private Methods
// ------------------------

	void RenderManager::RenderBeginFrame()
	{
		// Already handled by the device class module
		//glClearColor(0.471f, 0.741f, 0.596f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderManager::RenderEndFrame()
	{

	}



} // namespace pldn