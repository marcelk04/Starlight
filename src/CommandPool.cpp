#include "renderer/wrapper/CommandPool.hpp"

#include <stdexcept>

namespace stl {

CommandPool::CommandPool(Device& device, uint32_t queueFamilyIndex, CreateFlags flags)
	: m_Device{ device } {
	createCommandPool(queueFamilyIndex, flags);
}

CommandPool::~CommandPool() {
	vkDestroyCommandPool(m_Device.getDevice(), m_Pool, nullptr);
}

std::vector<VkCommandBuffer> CommandPool::allocateCommandBuffers(VkCommandBufferLevel level, uint32_t count) const {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = level;
	allocInfo.commandPool = m_Pool;
	allocInfo.commandBufferCount = count;

	std::vector<VkCommandBuffer> commandBuffers;

	vkAllocateCommandBuffers(m_Device.getDevice(), &allocInfo, commandBuffers.data());

	return commandBuffers;
}

void CommandPool::freeCommandBuffers(const std::vector<VkCommandBuffer>& commandBuffers) const {
	vkFreeCommandBuffers(m_Device.getDevice(), m_Pool, commandBuffers.size(), commandBuffers.data());
}

void CommandPool::createCommandPool(uint32_t queueFamilyIndex, CreateFlags flags) {
	QueueFamilyIndices indices = m_Device.findPhysicalQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndex;
	poolInfo.flags = flags;

	if (vkCreateCommandPool(m_Device.getDevice(), &poolInfo, nullptr, &m_Pool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

}