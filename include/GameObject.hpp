#pragma once

#include "renderer/Model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace stl {

struct TransformComponent {
	glm::vec3 translation;
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotation;

	glm::mat4 mat4() const {
		glm::mat4 transform{ 1.0f };

		// translation
		transform = glm::translate(transform, translation);

		// yxz rotation
		transform = glm::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
		transform = glm::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
		transform = glm::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

		// scaling
		transform = glm::scale(transform, scale);

		return transform;
	}
};

class GameObject {
public:
	using id_t = uint32_t;

public:
	GameObject(const GameObject&) = delete;
	GameObject(GameObject&&) = default;

	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(GameObject&&) = default;

	static GameObject createGameObject();

	id_t getId() const { return m_Id; };

private:
	GameObject(id_t objectId);

public:
	std::shared_ptr<Model> p_Model{};
	glm::vec3 p_Color{};
	TransformComponent p_Transform{};

private:
	id_t m_Id;
};

}