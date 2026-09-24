#pragma once

// Engine includes
#include <cstdint>
#include <vector>

namespace pldn
{
    struct SpriteFrame
    {
        std::uint16_t Column = 0;
        std::uint16_t Row = 0;
    };

    enum class AnimationPlaybackType
    {
        PlayLoop,
        PlayOnce
    };

    struct AnimationClip
    {
        std::vector<SpriteFrame> Frames = {};
        float FrameDurationSeconds = 0.1f;
        AnimationPlaybackType Playback = AnimationPlaybackType::PlayLoop;
    };
}