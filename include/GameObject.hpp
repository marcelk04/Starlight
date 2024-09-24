#pragma once

#include "Model.hpp"

#include <memory>

namespace stl {

struct Transform2DComponent {
	glm::vec2 translation;
	glm::vec2 scale{ 1.0f, 1.0f };
	float rotation;

	glm::mat2 mat2() {
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);

		glm::mat2 scaleMat{ {scale.x, 0.0f }, { 0.0f, scale.y } };
		glm::mat2 rotationMat{ { c, s }, { -s, c } };
		return rotationMat * scaleMat;
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
	Transform2DComponent p_Transform2D{};

private:
	id_t m_Id;
};

}