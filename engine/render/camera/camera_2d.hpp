#pragma once

// GLM includes
#include "glm/glm.hpp"
#include <glm/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace pldn
{

	class Camera2D
	{
	public:
		void SetPosition(const glm::vec2& position);
		void SetZoom(const float zoom);

		void Resize(unsigned int framebufferWidth, unsigned int framebufferHeight);

		glm::mat4 View() const;
		const glm::mat4& Projection() const;

	private:
		glm::mat4 m_projection = glm::mat4(1.0f);
		glm::vec2 m_position = glm::vec2(0.0f, 0.0f);

		float m_visibleHeight = 10.0f;
		float m_zoom = 1.0f;
	};


} // namespace pldn