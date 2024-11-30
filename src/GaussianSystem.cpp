#include "renderer/rendersystems/GaussianSystem.hpp"

#include "Core/Asserts.hpp"

#include <stdexcept>
#include <array>
#include <map>
#include <iterator>

namespace stl {

GaussianSystem::GaussianSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: m_Device{ device }, m_CubeModel{ Model::createModelFromFile(device, "assets/models/cube.obj") } {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

GaussianSystem::~GaussianSystem() {
	vkDestroyPipelineLayout(m_Device.getDevice(), m_PipelineLayout, nullptr);
}

void GaussianSystem::render(FrameInfo& frameInfo, GSPointCloud& pointCloud) {
	m_Pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

	m_CubeModel->bind(frameInfo.commandBuffer, 0);
	pointCloud.bind(frameInfo.commandBuffer, 1);
	m_CubeModel->draw(frameInfo.commandBuffer, pointCloud.getCount());
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

	pipelineConfig.bindingDescriptions = getBindingDescription();
	pipelineConfig.attributeDescriptions = getAttributeDescription();

	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = m_PipelineLayout;

	m_Pipeline = std::make_unique<Pipeline>(m_Device, "shaders/Gaussian.vert.spv", "shaders/Gaussian.frag.spv", pipelineConfig);
}

std::vector<VkVertexInputBindingDescription> GaussianSystem::getBindingDescription() const {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(2);

	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Model::Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	bindingDescriptions[1].binding = 1;
	bindingDescriptions[1].stride = sizeof(RichPoint);
	bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> GaussianSystem::getAttributeDescription() const {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(10);

	// Per Vertex Data
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Model::Vertex, position);

	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Model::Vertex, color);

	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Model::Vertex, normal);

	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Model::Vertex, uv);

	// Per Instance Data
	attributeDescriptions[4].location = 4;
	attributeDescriptions[4].binding = 1;
	attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[4].offset = offsetof(RichPoint, position);

	attributeDescriptions[5].location = 5;
	attributeDescriptions[5].binding = 1;
	attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[5].offset = offsetof(RichPoint, normal);

	attributeDescriptions[6].location = 6;
	attributeDescriptions[6].binding = 1;
	attributeDescriptions[6].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[6].offset = offsetof(RichPoint, shs);

	attributeDescriptions[7].location = 7;
	attributeDescriptions[7].binding = 1;
	attributeDescriptions[7].format = VK_FORMAT_R32_SFLOAT;
	attributeDescriptions[7].offset = offsetof(RichPoint, opacity);

	attributeDescriptions[8].location = 8;
	attributeDescriptions[8].binding = 1;
	attributeDescriptions[8].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[8].offset = offsetof(RichPoint, scale);

	attributeDescriptions[9].location = 9;
	attributeDescriptions[9].binding = 1;
	attributeDescriptions[9].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attributeDescriptions[9].offset = offsetof(RichPoint, rotation);

	return attributeDescriptions;
}

}
