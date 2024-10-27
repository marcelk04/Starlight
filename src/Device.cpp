#include "renderer/wrapper/Device.hpp"

#include "Core/Logger.hpp"

#include <cstring>
#include <set>
#include <unordered_set>

namespace stl {

Device::Device(Window& window)
	: m_Instance{}, m_Window{ window } {
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createCommandPool();
}

Device::~Device() {
	vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	vkDestroyDevice(m_Device, nullptr);

	vkDestroySurfaceKHR(m_Instance.getInstance(), m_Surface, nullptr);
}

SwapchainSupportDetails Device::getSwapchainSupport() {
	return querySwapchainSupport(m_PhysicalDevice);
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

QueueFamilyIndices Device::findPhysicalQueueFamilies() const {
	return findQueueFamilies(m_PhysicalDevice);
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format!");
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
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(m_Instance.getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	SINFO("Device count: ", deviceCount);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance.getInstance(), &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			m_PhysicalDevice = device;
			break;
		}
	}

	if (m_PhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}

	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &p_Properties);

	SINFO("Physical device: ", p_Properties.deviceName);
}

void Device::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

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

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
	createInfo.enabledLayerCount = 0; // could enable validation layers, ignored in up-to-date versions

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
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

bool Device::isDeviceSuitable(VkPhysicalDevice device) const {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapchainAdequate = false;
	if (extensionsSupported) {
		SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device);
		swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) const {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		i++;
	}

	return indices;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(std::string(extension.extensionName));
	}

	return requiredExtensions.empty();
}

SwapchainSupportDetails Device::querySwapchainSupport(VkPhysicalDevice device) const {
	SwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

}