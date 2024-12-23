#pragma once

#include "renderer/Bindable.hpp"
#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

namespace stl {

class Drawable : public Bindable {
public:
	Drawable() = default;

	virtual void draw(VkCommandBuffer commandBuffer, uint32_t instanceCount = 1) const = 0;
};

}