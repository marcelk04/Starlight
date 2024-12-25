#include "renderer/wrapper/Device.hpp"

#include "Core/Logger.hpp"

#include <cstring>
#include <set>
#include <unordered_set>
#include <utility>

namespace stl {

Device::Device(Window& window)
	: m_Instance{}, m_Window{ window } {
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createCommandPool();
}

Device::Device(Device&& other) noexcept
	: m_Instance{ std::move(other.m_Instance) },
	m_PhysicalDevice{ std::exchange(other.m_PhysicalDevice, nullptr) },
	m_CommandPool{ std::exchange(other.m_CommandPool, VK_NULL_HANDLE) },
	m_Window{ other.m_Window },
	m_Device{ std::exchange(other.m_Device, VK_NULL_HANDLE) },
	m_Surface{ std::exchange(other.m_Surface, VK_NULL_HANDLE) },
	m_GraphicsQueue{ std::exchange(other.m_GraphicsQueue, VK_NULL_HANDLE) },
	m_PresentQueue{ std::exchange(other.m_PresentQueue, VK_NULL_HANDLE) },
	m_ComputeQueue{ std::exchange(other.m_ComputeQueue, VK_NULL_HANDLE) },
	p_Properties{ std::move(other.p_Properties) } {
}

Device::~Device() {
	vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	vkDestroyDevice(m_Device, nullptr);

	vkDestroySurfaceKHR(m_Instance.getInstance(), m_Surface, nullptr);
}

Device& Device::operator=(Device&& other) noexcept {
	Device& copy(other);

	std::swap(m_Instance, copy.m_Instance);
	std::swap(m_PhysicalDevice, copy.m_PhysicalDevice);
	std::swap(m_CommandPool, copy.m_CommandPool);
	std::swap(m_Window, copy.m_Window);
	std::swap(m_Device, copy.m_Device);
	std::swap(m_Surface, copy.m_Surface);
	std::swap(m_GraphicsQueue, copy.m_GraphicsQueue);
	std::swap(m_PresentQueue, copy.m_PresentQueue);
	std::swap(m_ComputeQueue, copy.m_ComputeQueue);
	std::swap(p_Properties, copy.p_Properties);

	return *this;
}

SwapchainSupportDetails Device::getSwapchainSupport() {
	return m_PhysicalDevice->querySwapchainSupport();
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
	return m_PhysicalDevice->findMemoryType(typeFilter, properties);
}

QueueFamilyIndices Device::findPhysicalQueueFamilies() const {
	return m_PhysicalDevice->getQueueFamilies();
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
	return m_PhysicalDevice->findSupportedFormat(candidates, tiling, features);
}

void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
}

VkCommandBuffer Device::beginSingleTimeCommands() const {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_GraphicsQueue);

	vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
}

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layerCount;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
}

void Device::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) const {
	if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory!");
	}

	if (vkBindImageMemory(m_Device, image, imageMemory, 0) != VK_SUCCESS) {
		throw std::runtime_error("Failed to bind image memory!");
	}
}

void Device::createSurface() {
	m_Window.createWindowSurface(m_Instance.getInstance(), &m_Surface);
}

void Device::pickPhysicalDevice() {
	m_PhysicalDevice = std::make_shared<PhysicalDevice>(PhysicalDevice::suitableDevices(m_Instance, m_Surface)[0]);

	QueueFamilyIndices indices = m_PhysicalDevice->getQueueFamilies();
	SDEBUG("Graphics queue index: ", indices.graphicsFamily.value());
	SDEBUG("Present queue index: ", indices.presentFamily.value());
	SDEBUG("Compute queue index: ", indices.computeFamily.value());
}

void Device::createLogicalDevice() {
	QueueFamilyIndices indices = m_PhysicalDevice->getQueueFamilies();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value() };

	float queuePriority = 1.0f;

	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.shaderFloat64 = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
	createInfo.enabledLayerCount = 0; // could enable validation layers, ignored in up-to-date versions

	if (vkCreateDevice(m_PhysicalDevice->getPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
	vkGetDeviceQueue(m_Device, indices.computeFamily.value(), 0, &m_ComputeQueue);
}

void Device::createCommandPool() {
	QueueFamilyIndices indices = findPhysicalQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

}