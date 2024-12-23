#include "renderer/wrapper/Shader.hpp"

namespace stl {

Shader::Shader(Device& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath) 
	: m_Device{ device } {
	m_ShaderModules.reserve(2);
	m_ShaderModules.emplace_back(m_Device, vertexShaderPath, ShaderModule::Type::Vertex);
	m_ShaderModules.emplace_back(m_Device, fragmentShaderPath, ShaderModule::Type::Fragment);

	queryStageInfos();
}

Shader::Shader(Device& device, const std::string& computeShaderPath)
	: m_Device{ device } {
	m_ShaderModules.emplace_back(m_Device, computeShaderPath, ShaderModule::Type::Compute);

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