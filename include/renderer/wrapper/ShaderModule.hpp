#pragma once

#include "Core/Common.hpp"
#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

#include <string>

namespace stl {

class ShaderModule {
public:
    enum Type {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        Compute = VK_SHADER_STAGE_COMPUTE_BIT
    };

public:
    ShaderModule(Device& device, const std::string& path, Type type);
	ShaderModule(const ShaderModule&) = delete;
	ShaderModule(ShaderModule&& other) noexcept;
    ~ShaderModule();

    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule& operator=(ShaderModule&& other) noexcept;

    VkShaderModule getModule() const { return m_Module; }
    Type getType() const { return m_Type; }

    VkPipelineShaderStageCreateInfo getStageInfo() const;

private:
    void createModule();

private:
    Device& m_Device;

    std::string m_Path;
    VkShaderModule m_Module;
    Type m_Type;
};

}