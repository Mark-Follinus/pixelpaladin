#pragma once

// Standard library includes
#include <cstddef>

// Engine includes
#include "animation_clip.hpp"

namespace pldn
{
    class Animator2D
    {
    public:
        void Play(const AnimationClip* clip, const bool restart = false);

        void Stop();

        void Update(const float deltaTime);

        [[nodiscard]]
        const SpriteFrame* GetCurrentFrame() const noexcept;

        [[nodiscard]]
        bool IsFinished() const noexcept;

        [[nodiscard]]
        bool HasClip() const noexcept;

    private:
        const AnimationClip* m_currentClip = nullptr;

        std::size_t m_currentFrameIndex = 0;
        float m_frameElapsedTime = 0.0f;
        
        bool m_finished = false;
    };

} // namespace pldn