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

void FirstApp::loadGameObjects() {
	std::shared_ptr<Model> model = Model::createModelFromFile(m_Device, "assets/models/flat_vase.obj");

	GameObject gameObj = GameObject::createGameObject();
	gameObj.p_Model = model;
	gameObj.p_Transform.translation = { 0.0f, 0.0f, 0.0f };
	gameObj.p_Transform.scale = glm::vec3{ 3.0f };

	m_GameObjects.push_back(std::move(gameObj));
}

}