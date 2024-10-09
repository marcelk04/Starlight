#pragma once

#include "renderer/Model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace stl {

struct TransformComponent {
	glm::vec3 translation;
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotation;

	glm::mat4 modelMatrix() const;
	glm::mat3 normalMatrix() const;
};

class GameObject {
public:
	using id_t = uint32_t;
	using Map = std::unordered_map<id_t, GameObject>;

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