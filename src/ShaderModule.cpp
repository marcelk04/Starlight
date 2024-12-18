#include "renderer/wrapper/ShaderModule.hpp"

#include <stdexcept>
#include <utility>

namespace stl {

ShaderModule::ShaderModule(Device& device, const std::string& path, Type type) 
    : m_Device{ device }, m_Path{ path }, m_Type{ type } {
    createModule();
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept 
	: m_Device{ other.m_Device }, 
	m_Path{ std::move(other.m_Path) }, 
	m_Module{ std::exchange(other.m_Module, VK_NULL_HANDLE) }, 
	m_Type{ std::move(other.m_Type) } {
}

ShaderModule::~ShaderModule() {
    vkDestroyShaderModule(m_Device.getDevice(), m_Module, nullptr);
}

VkPipelineShaderStageCreateInfo ShaderModule::getPipelineShaderInfo() const {
    VkPipelineShaderStageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = static_cast<VkShaderStageFlagBits>(m_Type);
    info.module = m_Module;
    info.pName = "main";
    info.flags = 0;
    info.pSpecializationInfo = nullptr;
    info.pNext = nullptr;

    return info;
}

void ShaderModule::createModule() {
    auto shaderCode = Common::readFile(m_Path);

    VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	if (vkCreateShaderModule(m_Device.getDevice(), &createInfo, nullptr, &m_Module) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module!");
	}
}

}