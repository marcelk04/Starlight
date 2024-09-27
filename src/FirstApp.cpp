#include "FirstApp.hpp"

#include "Camera.hpp"

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

	//camera.setViewDirection(glm::vec3{ 0.0f }, glm::vec3{ 0.5f, 0.0f, 1.0f });
	camera.setViewTarget(glm::vec3{ -1.0f, -2.0f, 2.0f }, glm::vec3{ 0.0f, 0.0f, 2.5f });

	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!m_Window.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		std::cout << dt << std::endl;

		float aspect = m_Renderer.getAspectRatio();
		//camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
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

		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

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
	cube.p_Transform.translation = { 0.0f, 0.0f, 2.5f };
	cube.p_Transform.scale = { 0.5f, 0.5f, 0.5f };

	m_GameObjects.push_back(std::move(cube));
}

}