#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan_core.h>

namespace stl {

class CommandPool {
	enum CreateFlags {
		Transient = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		ResetCommandBuffer = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Protected = VK_COMMAND_POOL_CREATE_PROTECTED_BIT
	};

public:
	CommandPool(Device& device, uint32_t queueFamilyIndex, CreateFlags flags);
	~CommandPool();

	VkCommandPool getCommandPool() const { return m_Pool; }

	std::vector<VkCommandBuffer> allocateCommandBuffers(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, uint32_t count = 1) const;
	void freeCommandBuffers(const std::vector<VkCommandBuffer>& commandBuffers) const;

private:
	void createCommandPool(uint32_t queueFamilyIndex, CreateFlags flags);

private:
	Device& m_Device;

	VkCommandPool m_Pool;
};

}