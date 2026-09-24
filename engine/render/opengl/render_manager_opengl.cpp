#include "render_manager_opengl.hpp"

// Standard library includes
#include <cstddef>
#include <initializer_list>
#include <cstdint>
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
#include "render/sprites/sprite_2d.hpp"

namespace pldn
{
// ------------------------
// Local Functions
// ------------------------

	constexpr std::size_t ToIndex(const PlayerAnimationSpriteID id) noexcept
	{
		return static_cast<std::size_t>(id);
	}

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
				m_playerVisuals[i].reset();
				continue;
			}

			std::optional<PlayerVisual>& visualSlot = m_playerVisuals[i];

			if (!visualSlot.has_value())
			{
				PlayerVisual& newVisual = visualSlot.emplace();

				newVisual.Sprite.PositionOffset = { 0.0f, 0.0f };
				newVisual.Sprite.Scale = { 1.0f, 1.0f };
				newVisual.Sprite.RotationOffset = 0.0f;
				newVisual.Sprite.AtlasColumns = 2;
				newVisual.Sprite.AtlasRows = 4;
			}

			const PlayerState& state = players[i]->State;
			PlayerVisual& visual = *visualSlot;

			const AnimationClip& selectedClip = SelectPlayerAnimation(state);

			visual.Animator.Play(&selectedClip);
			visual.Animator.Update(deltaTime);

			const SpriteFrame* currentFrame = visual.Animator.GetCurrentFrame();

			if (currentFrame != nullptr)
			{
				visual.Sprite.AtlasColumn = currentFrame->Column;
				visual.Sprite.AtlasRow = currentFrame->Row;
			}
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

			const auto& players = world.GetPlayers();

			for (std::size_t i = 0; i < players.size(); ++i)
			{
				const auto& playerSlot = players[i];
				const auto& visualSlot = m_playerVisuals[i];

				if (!playerSlot.has_value() || !visualSlot.has_value())
				{
					continue;
				}

				const PlayerCharacter& player = *playerSlot;
				const PlayerVisual& visual = *visualSlot;

				m_renderer2D.DrawSprite(m_tempTexture, visual.Sprite, player.State.Transform);
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
		InitializePlayerAnimationClips();

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

	void RenderManager::InitializePlayerAnimationClips()
	{
		constexpr float frameDuration = 0.2f;
		const auto SetClip = [this, frameDuration](const PlayerAnimationSpriteID id, const std::initializer_list<SpriteFrame> frames)
		{
			AnimationClip& clip = m_playerAnimationClips[ToIndex(id)];
			clip.Frames.assign(frames.begin(), frames.end());
			clip.FrameDurationSeconds = frameDuration;
			clip.Playback = AnimationPlaybackType::PlayLoop;
		};

		// Idle Animations
		SetClip(PlayerAnimationSpriteID::IdleDown,  {SpriteFrame{.Column = 0, .Row = 0}});
		SetClip(PlayerAnimationSpriteID::IdleRight, {SpriteFrame{.Column = 0, .Row = 1}});
		SetClip(PlayerAnimationSpriteID::IdleLeft,  {SpriteFrame{.Column = 0, .Row = 2}});
		SetClip(PlayerAnimationSpriteID::IdleUp,    {SpriteFrame{.Column = 0, .Row = 3}});

		// Walking Animations
		SetClip(PlayerAnimationSpriteID::WalkDown,  {SpriteFrame{.Column = 0, .Row = 0}, SpriteFrame{.Column = 1, .Row = 0}});
		SetClip(PlayerAnimationSpriteID::WalkRight, {SpriteFrame{.Column = 0, .Row = 1}, SpriteFrame{.Column = 1, .Row = 1}});
		SetClip(PlayerAnimationSpriteID::WalkLeft,  {SpriteFrame{.Column = 0, .Row = 2}, SpriteFrame{.Column = 1, .Row = 2}});
		SetClip(PlayerAnimationSpriteID::WalkUp,    {SpriteFrame{.Column = 0, .Row = 3}, SpriteFrame{.Column = 1, .Row = 3}});
	}

	const AnimationClip& RenderManager::SelectPlayerAnimation(const PlayerState& state) const
	{
		const bool moving = state.Locomotion == PlayerLocomotion::Moving;

		PlayerAnimationSpriteID animationID = PlayerAnimationSpriteID::IdleDown;

		switch (state.Direction)
		{
			case PlayerDirection::Down:
			{
				animationID = moving ? PlayerAnimationSpriteID::WalkDown : PlayerAnimationSpriteID::IdleDown;
				break;
			}
			case PlayerDirection::Right:
			{
				animationID = moving ? PlayerAnimationSpriteID::WalkRight : PlayerAnimationSpriteID::IdleRight;
				break;
			}
			case PlayerDirection::Left:
			{
				animationID = moving ? PlayerAnimationSpriteID::WalkLeft : PlayerAnimationSpriteID::IdleLeft;
				break;
			}
			case PlayerDirection::Up:
			{
				animationID = moving ? PlayerAnimationSpriteID::WalkUp : PlayerAnimationSpriteID::IdleUp;
				break;
			}
		}

		return m_playerAnimationClips[ToIndex(animationID)];
	}

} // namespace pldn