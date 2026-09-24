#pragma once

// Engine includes
#include <optional>
#include <array>

// Engine includes
#include "render/camera/camera_2d.hpp"
#include "renderers/2d/renderer_2d_opengl.hpp"

// TEMP
#include "textures/texture_2d.hpp"
#include "game/constants/game_constants.hpp"
#include "render/entities/player_visual.hpp"
#include "render/animation/animation_clip.hpp"

namespace pldn 
{

	enum class PlayerAnimationSpriteID : std::uint8_t
	{
		IdleDown = 0,
		IdleRight = 1,
		IdleLeft = 2,
		IdleUp = 3,

		WalkDown = 4,
		WalkRight = 5,
		WalkLeft = 6,
		WalkUp = 7,

		Count = 8
	};

	// Forward declarations
	class GameWorldReplicated;
	struct PlayerState;

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

		void InitializePlayerAnimationClips();
		const AnimationClip& SelectPlayerAnimation(const PlayerState& state) const;

	private:
		friend class Engine;
		friend class ClientEngine;

		static constexpr std::size_t PLAYER_ANIMATION_CLIP_COUNT = static_cast<std::size_t>(PlayerAnimationSpriteID::Count);
		std::array<AnimationClip, PLAYER_ANIMATION_CLIP_COUNT> m_playerAnimationClips{};
		std::array<std::optional<PlayerVisual>, MAX_PLAYERS_ALLOWED> m_playerVisuals{};

		Camera2D m_worldCamera;
		Renderer2D m_renderer2D;

		Texture2D m_tempTexture;
	};
}