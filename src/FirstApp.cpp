#include "FirstApp.hpp"

#include "input/Input.hpp"
#include "renderer/wrapper/Buffer.hpp"
#include "Camera.hpp"
#include "KeyboardMovementController.hpp"

#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

namespace stl {

struct GlobalUbo {
	glm::mat4 projectionView{ 1.0f };
	glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f };
	glm::vec3 lightPosition{ -1.0f, 1.0f, 1.0f };
	alignas(16) glm::vec4 lightColor{ 1.0f }; // w is light intensity
};

FirstApp::FirstApp() {
	m_GlobalPool = DescriptorPool::Builder(m_Device)
		.setMaxSets(Swapchain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Swapchain::MAX_FRAMES_IN_FLIGHT)
		.build();

	loadGameObjects();
}

FirstApp::~FirstApp() {
}

void FirstApp::run() {
	std::vector<std::unique_ptr<Buffer>> uboBuffers(Swapchain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<Buffer>(m_Device,
			sizeof(GlobalUbo),
			Swapchain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		uboBuffers[i]->map();
	}

	auto globalSetLayout = DescriptorSetLayout::Builder(m_Device)
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		.build();

	std::vector<VkDescriptorSet> globalDescriptorSets(Swapchain::MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < globalDescriptorSets.size(); i++) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();

		DescriptorWriter(*globalSetLayout, *m_GlobalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}

	SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.getSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
	Camera camera{};

	GameObject viewerObject = GameObject::createGameObject();
	viewerObject.p_Transform.translation = { 0.0f, 0.0f, 2.5f };
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
		camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

		if (VkCommandBuffer commandBuffer = m_Renderer.beginFrame()) {
			int frameIndex = m_Renderer.getFrameIndex();
			FrameInfo frameInfo{ frameIndex, dt, commandBuffer, camera, globalDescriptorSets[frameIndex]};

			// update
			GlobalUbo ubo{};
			ubo.projectionView = camera.getProjection() * camera.getView();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			m_Renderer.beginSwapchainRenderPass(commandBuffer);

			simpleRenderSystem.renderGameObjects(frameInfo, m_GameObjects);

			m_Renderer.endSwapchainRenderPass(commandBuffer);
			m_Renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_Device.getDevice());
}

void FirstApp::loadGameObjects() {
	std::shared_ptr<Model> flatVaseModel = Model::createModelFromFile(m_Device, "assets/models/flat_vase.obj");

	GameObject flatVase = GameObject::createGameObject();
	flatVase.p_Model = flatVaseModel;
	flatVase.p_Transform.translation = { -0.5f, -0.5f, 0.0f };
	flatVase.p_Transform.scale = { 3.0f, -1.5f, 3.0f }; // negative y scale bc y axis of model is flipped
	m_GameObjects.push_back(std::move(flatVase));

	std::shared_ptr<Model> smoothVaseModel = Model::createModelFromFile(m_Device, "assets/models/smooth_vase.obj");

	GameObject smoothVase = GameObject::createGameObject();
	smoothVase.p_Model = smoothVaseModel;
	smoothVase.p_Transform.translation = { 0.5f, -0.5f, 0.0f };
	smoothVase.p_Transform.scale = { 3.0f, -1.5f, 3.0f };
	m_GameObjects.push_back(std::move(smoothVase));

	std::shared_ptr<Model> floorModel = Model::createModelFromFile(m_Device, "assets/models/quad.obj");

	GameObject floor = GameObject::createGameObject();
	floor.p_Model = floorModel;
	floor.p_Transform.translation = { 0.0f, -0.5f, 0.0f };
	floor.p_Transform.scale = { 3.0f, 1.0f, 3.0f };
	m_GameObjects.push_back(std::move(floor));
}

}