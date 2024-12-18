#include "renderer/wrapper/Shader.hpp"

#include "Core/Logger.hpp"

namespace stl {

Shader::Shader(Device& device, const std::string& vsFilepath, const std::string& fsFilepath) 
    : m_Device{ device } {
    m_ShaderModules.reserve(2);
    m_ShaderModules.emplace_back(m_Device, vsFilepath, ShaderModule::Type::Vertex);
    m_ShaderModules.emplace_back(m_Device, fsFilepath, ShaderModule::Type::Fragment);
}

Shader::Shader(Device& device, const std::string& csFilepath)
    : m_Device{ device } {
    m_ShaderModules.emplace_back(m_Device, csFilepath, ShaderModule::Type::Compute);
}

Shader::~Shader() {
}

std::vector<VkPipelineShaderStageCreateInfo> Shader::getPipelineShaderInfos() const {
    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderInfos;
    
    for (const ShaderModule& module : m_ShaderModules) {
        pipelineShaderInfos.push_back(module.getPipelineShaderInfo());
    }

    return pipelineShaderInfos;
}

}