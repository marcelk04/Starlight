#pragma once

#include "renderer/wrapper/Window.hpp"
#include "renderer/wrapper/Instance.hpp"

#include <string>
#include <vector>
#include <optional>

namespace stl {

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class Device {
public:
	Device(Window& window);
	~Device();

	Device(const Device&) = delete;
	Device(Device&&) = delete;

	Device& operator=(const Device&) = delete;
	Device& operator=(Device&&) = delete;

	VkCommandPool getCommandPool() const { return m_CommandPool; }
	VkDevice getDevice() const { return m_Device; }
	VkSurfaceKHR getSurface() const { return m_Surface; }
	VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }
	VkQueue getPresentQueue() const { return m_PresentQueue; }

	SwapchainSupportDetails getSwapchainSupport();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	QueueFamilyIndices findPhysicalQueueFamilies() const;
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
	VkCommandBuffer beginSingleTimeCommands() const;
	void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) const;

	void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;

private:
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();

	bool isDeviceSuitable(VkPhysicalDevice device) const;
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
	bool checkDeviceExtensionSupport(VkPhysicalDevice) const;
	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;

public:
	VkPhysicalDeviceProperties p_Properties;

private:
	Instance m_Instance;
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkCommandPool m_CommandPool;

	Window& m_Window;

	VkDevice m_Device;
	VkSurfaceKHR m_Surface;
	VkQueue m_GraphicsQueue;
	VkQueue m_PresentQueue;

	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}