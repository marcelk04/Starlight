#pragma once

#include "renderer/wrapper/Instance.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

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

class PhysicalDevice {
public:
	PhysicalDevice(Instance& instance, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
	~PhysicalDevice();

	VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }

	static std::vector<PhysicalDevice> suitableDevices(Instance& instance, VkSurfaceKHR surface);
	static std::vector<PhysicalDevice> enumeratePhysicalDevices(Instance& instance, VkSurfaceKHR surface);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	bool isSuitable() const;
	QueueFamilyIndices findQueueFamilies() const;
	bool supportsDeviceExtensions() const;
	SwapchainSupportDetails querySwapchainSupport() const;

public:
	VkPhysicalDeviceProperties p_Properties;

private:
	Instance& m_Instance;
	VkSurfaceKHR m_Surface;

	VkPhysicalDevice m_PhysicalDevice;

	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

}
