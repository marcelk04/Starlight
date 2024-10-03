#include "KeyboardMovementController.hpp"

#include <limits>

namespace stl {

void KeyboardMovementController::moveInPlaneXZ(float dt, GameObject& gameObject) const {
	glm::vec3 rotate{ 0.0f };

	if (Input::isKeyPressed(p_Keys.lookLeft)) rotate.y += 1.0f;
	if (Input::isKeyPressed(p_Keys.lookRight)) rotate.y -= 1.0f;
	if (Input::isKeyPressed(p_Keys.lookUp)) rotate.x += 1.0f;
	if (Input::isKeyPressed(p_Keys.lookDown)) rotate.x -= 1.0f;

	// only normalize (and rotate) if rotation vec is not 0
	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
		gameObject.p_Transform.rotation += p_LookSpeed * dt * glm::normalize(rotate);
	}

	// limit pitch between ~ +/- 85 degrees
	gameObject.p_Transform.rotation.x = glm::clamp(gameObject.p_Transform.rotation.x, -1.5f, 1.5f);
	gameObject.p_Transform.rotation.y = glm::mod(gameObject.p_Transform.rotation.y, glm::two_pi<float>());

	float yaw = gameObject.p_Transform.rotation.y;
	const glm::vec3 forwardDir{ -sin(yaw), 0.0f, -cos(yaw) };
	const glm::vec3 rightDir{ -forwardDir.z, 0.0f, forwardDir.x };
	const glm::vec3 upDir = { 0.0f, 1.0f, 0.0f };

	glm::vec3 move{ 0.0f };
	if (Input::isKeyPressed(p_Keys.moveForward)) move += forwardDir;
	if (Input::isKeyPressed(p_Keys.moveBackward)) move -= forwardDir;
	if (Input::isKeyPressed(p_Keys.moveRight)) move += rightDir;
	if (Input::isKeyPressed(p_Keys.moveLeft)) move -= rightDir;
	if (Input::isKeyPressed(p_Keys.moveUp)) move += upDir;
	if (Input::isKeyPressed(p_Keys.moveDown)) move -= upDir;

	// only normalize (and move) if move vec is not 0
	if (glm::dot(move, move) > std::numeric_limits<float>::epsilon()) {
		gameObject.p_Transform.translation += p_MoveSpeed * dt * glm::normalize(move);
	}
}

}