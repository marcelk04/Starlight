#include "Camera.hpp"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <limits>

namespace stl {

void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
	m_ProjectionMatrix = glm::ortho(left, right, top, bottom, near, far);
}

void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
	assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

	m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);

}

}