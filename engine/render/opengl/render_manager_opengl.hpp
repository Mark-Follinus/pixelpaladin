#pragma once

// Engine includes
#include "camera/camera_2d.hpp"
#include "renderers/2d/renderer_2d_opengl.hpp"

// TEMP
#include "textures/texture_2d.hpp"
#include <array>
#include "game/constants/game_constants.hpp"

namespace pldn 
{

	enum class SpriteDirection : unsigned int
	{
		Down  = 0,
		Right = 1,
		Left  = 2,
		Up    = 3
	};

	struct PlayerAnimationState
	{
		SpriteDirection Direction = SpriteDirection::Down;

		unsigned int Frame = 0;
		float FrameTimer = 0.0f;

		glm::vec2 PreviousPosition{ 0.0f, 0.0f };
		float TimeSinceMovement = 0.0f;

		bool Initialized = false;
	};

	// Forward declarations
	class GameWorldReplicated;

	class RenderManager
	{
	public:
		void UpdateAnimations(const GameWorldReplicated& world, float deltaTime);
		void DrawScene(const GameWorldReplicated& world);
		void Resize(const unsigned int width, const unsigned int height);

	private:
		RenderManager();
		~RenderManager();

		void RenderBeginFrame();
		void RenderEndFrame();

		void UpdatePlayerAnimation(PlayerAnimationState& animation, const glm::vec2& position, float deltaTime);

	private:
		friend class Engine;
		friend class ClientEngine;

		Camera2D m_worldCamera;
		Renderer2D m_renderer2D;

		Texture2D m_tempTexture;

		std::array<PlayerAnimationState, MAX_PLAYERS_ALLOWED> m_playerAnimations;
	};
}