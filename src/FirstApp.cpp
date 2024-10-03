#include "FirstApp.hpp"

#include "input/Input.hpp"
#include "Camera.hpp"
#include "KeyboardMovementController.hpp"

#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

namespace stl {

FirstApp::FirstApp() {
	loadGameObjects();
}

FirstApp::~FirstApp() {
}

void FirstApp::run() {
	SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.getSwapchainRenderPass() };
	Camera camera{};

	GameObject viewerObject = GameObject::createGameObject();
	viewerObject.p_Transform.translation = { 0.0f, 0.0f, 2.0f };
	KeyboardMovementController cameraController{};

	//camera.setViewTarget(glm::vec3{0.0f, 0.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f });

	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_Window.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.moveInPlaneXZ(dt, viewerObject);
		camera.setViewYXZ(viewerObject.p_Transform.translation, viewerObject.p_Transform.rotation);

		float aspect = m_Renderer.getAspectRatio();
		camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

		if (VkCommandBuffer commandBuffer = m_Renderer.beginFrame()) {
			m_Renderer.beginSwapchainRenderPass(commandBuffer);

			simpleRenderSystem.renderGameObjects(commandBuffer, m_GameObjects, camera);

			m_Renderer.endSwapchainRenderPass(commandBuffer);
			m_Renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_Device.getDevice());
}

std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
	std::vector<Model::Vertex> vertices{
		// front face +z (red)
		{{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},

		// top face +y (green)
		{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
		{{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
		{{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

		// right face +x (blue)
		{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
		{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
		{{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},

		// back face -z (yellow)
		{{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},

		// bottom face -y (cyan)
		{{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}},
		{{1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}},
		{{1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 1.0f}},
		{{1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
		{{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},

		// left face -x (magenta)
		{{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
		{{-1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}},
		{{-1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 1.0f}},
		{{-1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}
	};

	for (auto& v : vertices) {
		v.position += offset;
	}

	return std::make_unique<Model>(device, vertices);
}

void FirstApp::loadGameObjects() {
	std::shared_ptr<Model> model = createCubeModel(m_Device, glm::vec3{ 0.0f, 0.0f, 0.0f });

	GameObject cube = GameObject::createGameObject();
	cube.p_Model = model;
	cube.p_Transform.translation = { 0.0f, 0.0f, 0.0f };
	cube.p_Transform.scale = { 0.25f, 0.25f, 0.25f };

	m_GameObjects.push_back(std::move(cube));
}

}