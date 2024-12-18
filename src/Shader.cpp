#include "renderer/wrapper/Shader.hpp"

namespace stl {

Shader::Shader(Device& device, const std::string& vsFilepath, const std::string& fsFilepath) 
    : m_Device{ device } {
    m_ShaderModules.reserve(2);
    m_ShaderModules.emplace_back(m_Device, vsFilepath, ShaderModule::Type::Vertex);
    m_ShaderModules.emplace_back(m_Device, fsFilepath, ShaderModule::Type::Fragment);

	queryStageInfos();
}

Shader::Shader(Device& device, const std::string& csFilepath)
    : m_Device{ device } {
    m_ShaderModules.emplace_back(m_Device, csFilepath, ShaderModule::Type::Compute);

	queryStageInfos();
}

Shader::~Shader() {
}

void Shader::queryStageInfos() {
	m_StageInfos.reserve(m_ShaderModules.size());

	for (const ShaderModule& module : m_ShaderModules) {
		m_StageInfos.push_back(module.getStageInfo());
	}
}

}