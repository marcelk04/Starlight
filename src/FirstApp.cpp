#include "FirstApp.hpp"

#include <stdexcept>

namespace stl {

FirstApp::FirstApp() {
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
	}
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
}

void FirstApp::drawFrame() {
}

}