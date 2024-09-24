#include "FirstApp.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace stl {

FirstApp::FirstApp() {
	loadGameObjects();
	createPipelineLayout();
	recreateSwapchain();
	createCommandBuffers();
}

FirstApp::~FirstApp() {
	vkDestroyPipelineLayout(m_Device.getDevice(), m_PipelineLayout, nullptr);
}

void FirstApp::run() {
	while (!m_Window.shouldClose()) {
		glfwPollEvents();
		drawFrame();
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

void FirstApp::createPipelineLayout() {
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(m_Device.getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void FirstApp::createPipeline() {
	assert(m_Swapchain != nullptr && "Cannot create pipeline before swapchain!");
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.renderPass = m_Swapchain->getRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/SimpleShader.vert.spv", "shaders/SimpleShader.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {
	m_CommandBuffers.resize(m_Swapchain->imageCount());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.getDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}

void FirstApp::freeCommandBuffers() {
	vkFreeCommandBuffers(m_Device.getDevice(), m_Device.getCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

void FirstApp::drawFrame() {
	uint32_t imageIndex;
	auto result = m_Swapchain->acquireNextImage(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swapchain image!");
	}

	recordCommandBuffer(imageIndex);

	result = m_Swapchain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
		m_Window.resetWindowResizedFlag();
		recreateSwapchain();
		return;
	}

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image!");
	}
}

void FirstApp::recreateSwapchain() {
	VkExtent2D extent = m_Window.getExtent();

	while (extent.width == 0 || extent.height == 0) {
		extent = m_Window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_Device.getDevice());

	if (m_Swapchain == nullptr) {
		m_Swapchain = std::make_unique<Swapchain>(m_Device, extent);
	} else {
		m_Swapchain = std::make_unique<Swapchain>(m_Device, extent, std::move(m_Swapchain));

		if (m_Swapchain->imageCount() != m_CommandBuffers.size()) {
			freeCommandBuffers();
			createCommandBuffers();
		}
	}

	createPipeline();
}

void FirstApp::recordCommandBuffer(int imageIndex) {
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_Swapchain->getRenderPass();
	renderPassInfo.framebuffer = m_Swapchain->getFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_Swapchain->getSwapchainExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_Swapchain->getSwapchainExtent().width);
	viewport.height = static_cast<float>(m_Swapchain->getSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Swapchain->getSwapchainExtent();

	vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

	renderGameObjects(m_CommandBuffers[imageIndex]);

	vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

	if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}

void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
	m_Pipeline->bind(commandBuffer);

	for (auto& obj : m_GameObjects) {
		SimplePushConstantData push = {};
		push.transform = obj.p_Transform2D.mat2();
		push.offset = obj.p_Transform2D.translation;
		push.color = obj.p_Color;

		vkCmdPushConstants(commandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		obj.p_Model->bind(commandBuffer);
		obj.p_Model->draw(commandBuffer);
	}
}

}