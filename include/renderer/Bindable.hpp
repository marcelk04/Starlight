#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

namespace stl {

class Bindable {
public:
	Bindable() = default;
	virtual ~Bindable() = default;

	virtual void bind(VkCommandBuffer commandBuffer, uint32_t firstBinding = 0) = 0;
};

}