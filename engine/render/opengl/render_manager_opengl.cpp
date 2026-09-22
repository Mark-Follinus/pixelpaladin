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

#include "game/components/world_transform_2d.hpp"
#include "data/sprite_2d.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void RenderManager::UpdateAnimations(const GameWorldReplicated& world, float deltaTime)
	{
		const auto& players = world.GetPlayers();

		for (std::size_t i = 0; i < players.size(); ++i)
		{
			if (!players[i].has_value())
			{
				// Reset state so a future player reusing this
				// slot doesn't inherit the old animation.
				m_playerAnimations[i] = PlayerAnimationState{};

				continue;
			}

			const PlayerState& player = players[i].value();
			PlayerAnimationState& animation = m_playerAnimations[i];
			UpdatePlayerAnimation(animation, player.Transform.Position, deltaTime);
		}
	}


	void RenderManager::DrawScene(const GameWorldReplicated& world)
	{
		/* BEGIN FRAME */
		RenderBeginFrame();

		/* DRAW WORLD*/

		if (m_renderer2D.IsInitialized())
		{
			m_renderer2D.StartScene(m_worldCamera);

			for (std::size_t i = 0; i < world.GetPlayers().size(); ++i)
			{
				if (!world.GetPlayers()[i].has_value())
				{
					continue;
				}

				SpriteTransform sprite;
				sprite.PositionOffset = { 0.0f, 0.0f };
				sprite.         Scale = { 1.0f, 1.0f };
				sprite.RotationOffset = 0.0f;
				sprite.   AtlasColumn = m_playerAnimations[i].Frame;
				sprite.      AtlasRow = static_cast<unsigned int>(m_playerAnimations[i].Direction);
				sprite.  AtlasColumns = 2;
				sprite.     AtlasRows = 4;

				m_renderer2D.DrawSprite(m_tempTexture, sprite, world.GetPlayers()[i]->Transform);
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

	void RenderManager::UpdatePlayerAnimation(PlayerAnimationState& animation, const glm::vec2& position, float deltaTime)
	{
		if (!animation.Initialized)
		{
			animation.PreviousPosition = position;
			animation.Initialized = true;
			return;
		}

		const glm::vec2 movement = position - animation.PreviousPosition;
		animation.PreviousPosition = position;
		constexpr float movementEpsilon = 0.0001f;
		const bool positionChanged = glm::dot(movement, movement) > movementEpsilon * movementEpsilon;

		if (positionChanged)
		{
			animation.TimeSinceMovement = 0.0f;

			if (std::abs(movement.x) > std::abs(movement.y))
			{
				animation.Direction = movement.x > 0.0f ? SpriteDirection::Right : SpriteDirection::Left;
			}
			else
			{
				animation.Direction = movement.y > 0.0f ? SpriteDirection::Up : SpriteDirection::Down;
			}
		}
		else
		{
			animation.TimeSinceMovement += deltaTime;
		}

		/*
		 * The world may only receive a new position during fixed
		 * ticks, while rendering can happen more frequently.
		 * Therefore, don't declare the player idle after one
		 * render frame without movement.
		 */
		constexpr float movementTimeout = 0.1f;
		const bool walking = animation.TimeSinceMovement < movementTimeout;
		constexpr float frameDuration = 0.2f;

		if (walking)
		{
			animation.FrameTimer += deltaTime;

			while (animation.FrameTimer >= frameDuration)
			{
				animation.FrameTimer -= frameDuration;
				animation.Frame = (animation.Frame + 1) % 2;
			}
		}
		else
		{
			animation.Frame = 0;
			animation.FrameTimer = 0.0f;
		}
	}


} // namespace pldn