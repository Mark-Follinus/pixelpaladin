#include "camera_2d.hpp"

// GLM includes
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace pldn
{

// ------------------------
// Public Methods
// ------------------------

	void Camera2D::SetPosition(const glm::vec2& position)
	{
		m_position = position;
	}

	void Camera2D::SetZoom(const float zoom)
	{
		m_zoom = zoom;
	}

	void Camera2D::Resize(unsigned int width, unsigned int height)
	{
		if (width == 0 || height == 0)
		{
			return;
		}

		const float aspect = static_cast<float>(width) / static_cast<float>(height);

		const float halfHeight = (m_visibleHeight * 0.5f) / m_zoom;
		const float halfWidth = halfHeight * aspect;

		m_projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
	}

	glm::mat4 Camera2D::View() const
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(-m_position, 0.0f));
	}

	const glm::mat4& Camera2D::Projection() const
	{
		return m_projection;
	}

} // namespace pldn