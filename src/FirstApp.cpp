#include "FirstApp.hpp"

#include <stdexcept>
#include <array>

namespace stl {

FirstApp::FirstApp() {
	loadModels();
	createPipelineLayout();
	createPipeline();
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
		{{ 0.0f, -0.5f }},
		{{ 0.5f, 0.5f }},
		{{ -0.5f, 0.5f }}
	};

	m_Model = std::make_unique<Model>(m_Device, vertices);
}

void FirstApp::createPipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_Device.getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void FirstApp::createPipeline() {
	auto pipelineConfig = Pipeline::defaultPipelineConfigInfo(m_Swapchain.getWidth(), m_Swapchain.getHeight());

	pipelineConfig.renderPass = m_Swapchain.getRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/SimpleShader.vert.spv", "shaders/SimpleShader.frag.spv", pipelineConfig);
}

void FirstApp::createCommandBuffers() {
	m_CommandBuffers.resize(m_Swapchain.imageCount());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.getDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_Swapchain.getRenderPass();
		renderPassInfo.framebuffer = m_Swapchain.getFramebuffer(i);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_Swapchain.getSwapchainExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_Pipeline->bind(m_CommandBuffers[i]);
		m_Model->bind(m_CommandBuffers[i]);
		m_Model->draw(m_CommandBuffers[i]);

		vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void FirstApp::drawFrame() {
	uint32_t imageIndex;
	auto result = m_Swapchain.acquireNextImage(&imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swapchain image!");
	}

	result = m_Swapchain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image!");
	}
}

}