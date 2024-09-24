#include "FirstApp.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace stl {

FirstApp::FirstApp() {
	loadGameObjects();
}

FirstApp::~FirstApp() {
}

void FirstApp::run() {
	SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.getSwapchainRenderPass() };

	while (!m_Window.shouldClose()) {
		glfwPollEvents();

		if (VkCommandBuffer commandBuffer = m_Renderer.beginFrame()) {
			m_Renderer.beginSwapchainRenderPass(commandBuffer);

			simpleRenderSystem.renderGameObjects(commandBuffer, m_GameObjects);

			m_Renderer.endSwapchainRenderPass(commandBuffer);
			m_Renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_Device.getDevice());
}

void FirstApp::loadGameObjects() {
	std::vector<Model::Vertex> vertices = {
		{{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
		{{ 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
		{{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
	};

	auto model = std::make_shared<Model>(m_Device, vertices);

	auto triangle = GameObject::createGameObject();
	triangle.p_Model = model;
	triangle.p_Color = { 0.1f, 0.8f, 0.1f };
	triangle.p_Transform2D.translation.x = 0.2f;
	triangle.p_Transform2D.scale = { 2.0f, 0.5f };
	triangle.p_Transform2D.rotation = 0.25f * glm::two_pi<float>();

	m_GameObjects.push_back(std::move(triangle));
}

}