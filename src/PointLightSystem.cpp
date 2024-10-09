#include "renderer/rendersystems/PointLightSystem.hpp"

#include <cassert>
#include <stdexcept>
#include <array>

namespace stl {

PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: m_Device{ device } {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
	vkDestroyPipelineLayout(m_Device.getDevice(), m_PipelineLayout, nullptr);
}

void PointLightSystem::render(FrameInfo& frameInfo) {
	m_Pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
	/*
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);
	*/

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_Device.getDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);

	pipelineConfig.bindingDescriptions.clear();
	pipelineConfig.attributeDescriptions.clear();

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/PointLight.vert.spv", "shaders/PointLight.frag.spv", pipelineConfig);
}

}