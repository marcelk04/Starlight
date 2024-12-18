#pragma once

#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/ShaderModule.hpp"

#include <vulkan/vulkan.h>

#include <string>

namespace stl {

class Shader {
public:
    Shader(Device& device, const std::string& vsFilepath, const std::string& fsFilepath);
    Shader(Device& device, const std::string& csFilepath);
    ~Shader();

    std::vector<VkPipelineShaderStageCreateInfo> getPipelineShaderInfos() const;

private:
    Device& m_Device;

    std::vector<ShaderModule> m_ShaderModules;
};

}