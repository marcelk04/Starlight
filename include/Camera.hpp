#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace stl {

class Camera {
public:
	void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
	void setPerspectiveProjection(float fovy, float aspect, float near, float far);

	const glm::mat4& getProjection() const { return m_ProjectionMatrix; }

private:
	glm::mat4 m_ProjectionMatrix{ 1.0f };
};

}