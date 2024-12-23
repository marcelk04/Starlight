#pragma once

#include "renderer/Bindable.hpp"
#include "renderer/Drawable.hpp"
#include "renderer/Model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>
#include <optional>

namespace stl {

struct TransformComponent {
	glm::vec3 translation;
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotation;

	glm::mat4 modelMatrix() const;
	glm::mat3 normalMatrix() const;
};

struct PointLightComponent {
	float lightIntensity = 1.0f;
};

struct InstanceComponent {
	std::shared_ptr<Bindable> instanceData{ nullptr };
	uint32_t instanceCount{ 1 };
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
	static GameObject createPointLight(float intensity = 1.0f, float radius = 0.1f, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

	id_t getId() const { return m_Id; };

private:
	GameObject(id_t objectId);

public:
	std::shared_ptr<Drawable> p_Model{};
	glm::vec3 p_Color{};

	TransformComponent p_Transform{};
	std::optional<PointLightComponent> p_PointLight = std::nullopt;
	std::optional<InstanceComponent> p_Instance = std::nullopt;

private:
	id_t m_Id;
};

}