#include "FirstApp.hpp"

#include <stdexcept>
#include <array>
#include <iostream>

namespace stl {

FirstApp::FirstApp() {
	loadModels();
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

void FirstApp::loadModels() {
	std::vector<Model::Vertex> vertices = {
		{{ 0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
		{{ 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }},
		{{ -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }}
	};

	m_Model = std::make_unique<Model>(m_Device, vertices);
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
	static int frame = 0;
	frame = (frame + 1) % 1000;

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

	m_Pipeline->bind(m_CommandBuffers[imageIndex]);
	m_Model->bind(m_CommandBuffers[imageIndex]);

	for (int j = 0; j < 4; j++) {
		SimplePushConstantData push = {};
		push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2 + j * 0.2f };

		vkCmdPushConstants(m_CommandBuffers[imageIndex], m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);

		m_Model->draw(m_CommandBuffers[imageIndex]);
	}

	vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);

	if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}

}