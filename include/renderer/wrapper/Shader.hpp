#pragma once

#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/ShaderModule.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

namespace stl {

class Shader {
public:
	Shader(Device& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	Shader(Device& device, const std::string& computeShaderPath);
	~Shader();

	const std::vector<ShaderModule>& getShaderModules() const { return m_ShaderModules; }
	const std::vector<VkPipelineShaderStageCreateInfo>& getStageInfos() const { return m_StageInfos; }

private:
	void queryStageInfos();

private:
	Device& m_Device;

	std::vector<ShaderModule> m_ShaderModules;
	std::vector<VkPipelineShaderStageCreateInfo> m_StageInfos;
};

}