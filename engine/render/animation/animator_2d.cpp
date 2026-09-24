#include "animator_2d.hpp"

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void Animator2D::Play(const AnimationClip* clip, const bool restart)
	{
		if (clip == nullptr || clip->Frames.empty() || clip->FrameDurationSeconds <= 0.0f)
		{
			Stop();
			return;
		}

		if (m_currentClip == clip && !restart)
		{
			return;
		}

		m_currentClip = clip;
		m_currentFrameIndex = 0;
		m_frameElapsedTime = 0.0f;
		m_finished = false;
	}

	void Animator2D::Stop()
	{
		m_currentClip = nullptr;
		m_currentFrameIndex = 0;
		m_frameElapsedTime = 0.0f;
		m_finished = false;
	}

	void Animator2D::Update(const float deltaTime)
	{
		if (m_currentClip == nullptr || m_finished || deltaTime <= 0.0f)
		{
			return;
		}

		m_frameElapsedTime += deltaTime;
		const float frameDuration = m_currentClip->FrameDurationSeconds;

		while (m_frameElapsedTime >= frameDuration)
		{
			m_frameElapsedTime -= frameDuration;

			const bool hasNextFrame = m_currentFrameIndex + 1 < m_currentClip->Frames.size();

			if (hasNextFrame)
			{
				m_currentFrameIndex++;
				continue;
			}

			if (m_currentClip->Playback == AnimationPlaybackType::PlayLoop)
			{
				m_currentFrameIndex = 0;
				continue;
			}

			// PlayOnce: remain on the final frame.
			m_currentFrameIndex = m_currentClip->Frames.size() - 1;
			m_frameElapsedTime = 0.0f;
			m_finished = true;
			break;
		}
	}

	const SpriteFrame* Animator2D::GetCurrentFrame() const noexcept
	{
		if (m_currentClip == nullptr || m_currentClip->Frames.empty())
		{
			return nullptr;
		}

		return &m_currentClip->Frames[m_currentFrameIndex];
	}

	bool Animator2D::IsFinished() const noexcept
	{
		return m_finished;
	}

	bool Animator2D::HasClip() const noexcept
	{
		return m_currentClip != nullptr;
	}

} // namespace pldn