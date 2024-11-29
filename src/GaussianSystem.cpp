#include "renderer/rendersystems/GaussianSystem.hpp"

#include "Core/Asserts.hpp"

#include <stdexcept>
#include <array>
#include <map>
#include <iterator>

namespace stl {

GaussianSystem::GaussianSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: m_Device{ device } {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

GaussianSystem::~GaussianSystem() {
	vkDestroyPipelineLayout(m_Device.getDevice(), m_PipelineLayout, nullptr);
}

void GaussianSystem::render(FrameInfo& frameInfo, GSPointCloud& pointCloud) {
	m_Pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	pointCloud.bind(frameInfo.commandBuffer);
	pointCloud.draw(frameInfo.commandBuffer);
}

void GaussianSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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

void GaussianSystem::createPipeline(VkRenderPass renderPass) {
	SASSERT_MSG(m_PipelineLayout != nullptr, "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	Pipeline::enableAlphaBlend(pipelineConfig);

	pipelineConfig.bindingDescriptions = RichPoint::getBindingDescription();
	pipelineConfig.attributeDescriptions = RichPoint::getAttributeDescription();

	pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/Gaussian.vert.spv", "shaders/Gaussian.frag.spv", pipelineConfig);
}

}
