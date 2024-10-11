#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace stl {

class Camera {
public:
	void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
	void setPerspectiveProjection(float fovy, float aspect, float near, float far);

	void setViewDirection(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& up = { 0.0f, 1.0f, 0.0f });
	void setViewTarget(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = { 0.0f, 1.0f, 0.0f });
	void setViewYXZ(const glm::vec3& position, const glm::vec3& rotation);

	const glm::mat4& getProjection() const { return m_ProjectionMatrix; }
	const glm::mat4& getView() const { return m_ViewMatrix; }
	const glm::mat4& getInverseView() const { return m_InverseViewMatrix; }
	const glm::vec3& getPosition() const { return glm::vec3(m_InverseViewMatrix[3]); }

private:
	glm::mat4 m_ProjectionMatrix{ 1.0f };
	glm::mat4 m_ViewMatrix{ 1.0f };
	glm::mat4 m_InverseViewMatrix{ 1.0f };
};

}