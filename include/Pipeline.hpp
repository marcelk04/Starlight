#pragma once

#include "Device.hpp"

#include <string>
#include <vector>

namespace stl {

struct PipelineConfigInfo {
	PipelineConfigInfo() = default;

	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class Pipeline {
public:
	Pipeline(Device& device, const std::string& vsFilepath, const std::string& fsFilepath, const PipelineConfigInfo& configInfo);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;

	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	void bind(VkCommandBuffer commandBuffer) const;

private:
	static std::vector<char> readFile(const std::string& filepath);

	void createGraphicsPipeline(const std::string& vsFilepath, const std::string& fsFilepath, const PipelineConfigInfo& configInfo);

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

private:
	Device& m_Device;

	VkPipeline m_Pipeline;
	VkShaderModule m_VertShaderModule;
	VkShaderModule m_FragShaderModule;
};

}