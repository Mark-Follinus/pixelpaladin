#include "render_manager_opengl.hpp"

// Standard library includes
#include <optional>
#include <string>

// GLFW & GLAD libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM libraries
#include "glm/glm.hpp"

// Engine includes
#include "game/world/replicated/game_world_replicated.hpp"

#include "game/data/world_transform_2d.hpp"
#include "data/sprite_2d.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void RenderManager::DrawScene(const GameWorldReplicated& world)
	{
		/* BEGIN FRAME */
		RenderBeginFrame();

		/* DRAW WORLD*/

		if (m_renderer2D.IsInitialized())
		{
			m_renderer2D.StartScene(m_worldCamera);

			for (const std::optional<PlayerState>& player : world.GetPlayers())
			{
				if (!player.has_value())
				{
					continue;
				}

				SpriteTransform sprite;
				sprite.PositionOffset = { 0.0f, 0.0f };
				sprite.         Scale = { 1.0f, 1.0f };
				sprite.RotationOffset = 0.0f;
				sprite.   AtlasColumn = 0;
				sprite.      AtlasRow = 2;
				sprite.  AtlasColumns = 2;
				sprite.     AtlasRows = 4;

				m_renderer2D.DrawSprite(m_tempTexture, sprite, player->Transform);

				//m_renderer2D.DrawQuad(player->Transform.Position, glm::vec2(1.0f, 1.0f), glm::radians(player->Transform.Rotation));
			}

			m_renderer2D.EndScene();
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

	void RenderManager::Resize(const unsigned int width, const unsigned int height)
	{
		m_worldCamera.Resize(width, height);
	}

// ------------------------
// Private Constructors
// ------------------------

	RenderManager::RenderManager() 
	{
		m_renderer2D.Initialize();
		m_tempTexture.Load("D:/GitHub/pixel-paladin/assets/sprites/testing/test_avatar.png");
	}

	RenderManager::~RenderManager() {}

// ------------------------
// Private Methods
// ------------------------

	void RenderManager::RenderBeginFrame()
	{
		// Already handled by the device class module
		glClearColor(0.471f, 0.741f, 0.596f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderManager::RenderEndFrame()
	{

	}



} // namespace pldn