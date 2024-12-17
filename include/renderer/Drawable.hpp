#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

namespace stl {

class Drawable {
public:
	Drawable(Device& device) : m_Device{ device } {}
	virtual ~Drawable() = default;

	virtual void bind(VkCommandBuffer commandBuffer, uint32_t firstBinding = 0) = 0;
	virtual void draw(VkCommandBuffer commandBuffer, uint32_t instanceCount = 1) const = 0;

protected:
	Device& m_Device;
};

}