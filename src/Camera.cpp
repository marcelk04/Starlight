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


void Camera::setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up) {
	setViewTarget(position, position + direction, up);
}

void Camera::setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
	// rotate the coordinate system by 180 degrees around the x axis (bc why not?)
	// this should probably be fixed in the future
	glm::mat4 m{ 1.0f };
	m[1][1] = -1.0f;
	m[2][2] = -1.0f;

	m_ViewMatrix = glm::lookAt(position, target, up) * m;
}

void Camera::setViewYXZ(const glm::vec3& position, const glm::vec3& rotation) {
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);
	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
	m_ViewMatrix = glm::mat4{ 1.f };
	m_ViewMatrix[0][0] = u.x;
	m_ViewMatrix[1][0] = u.y;
	m_ViewMatrix[2][0] = u.z;
	m_ViewMatrix[0][1] = v.x;
	m_ViewMatrix[1][1] = v.y;
	m_ViewMatrix[2][1] = v.z;
	m_ViewMatrix[0][2] = w.x;
	m_ViewMatrix[1][2] = w.y;
	m_ViewMatrix[2][2] = w.z;
	m_ViewMatrix[3][0] = -glm::dot(u, position);
	m_ViewMatrix[3][1] = -glm::dot(v, position);
	m_ViewMatrix[3][2] = -glm::dot(w, position);
}

}