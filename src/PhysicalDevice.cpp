#include "renderer/wrapper/PhysicalDevice.hpp"

#include "Core/Logger.hpp"

#include <stdexcept>
#include <sstream>
#include <set>
#include <string>

namespace stl {

PhysicalDevice::PhysicalDevice(Instance& instance, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
	: m_Instance{ instance }, m_Surface{ surface }, m_PhysicalDevice {
	physicalDevice
} {
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &p_Properties);
}

PhysicalDevice::~PhysicalDevice() {
}

std::vector<PhysicalDevice> PhysicalDevice::suitableDevices(Instance& instance, VkSurfaceKHR surface) {
	std::vector<PhysicalDevice> availableDevices = enumeratePhysicalDevices(instance, surface);
	std::vector<PhysicalDevice> suitableDevices;

	std::stringstream ss;
	ss << "Suitable devices: ";

	for (PhysicalDevice& device : availableDevices) {
		if (device.isSuitable()) {
			ss << device.p_Properties.deviceName << ' ';

			suitableDevices.push_back(std::move(device));
		}
	}

	if (suitableDevices.empty()) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}

	SINFO(ss.str());

	return suitableDevices;
}

std::vector<PhysicalDevice> PhysicalDevice::enumeratePhysicalDevices(Instance& instance, VkSurfaceKHR surface) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	SINFO("Device count: ", deviceCount);

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, devices.data());

	std::vector<PhysicalDevice> physicalDevices;
	physicalDevices.reserve(deviceCount);

	for (VkPhysicalDevice device : devices) {
		physicalDevices.emplace_back(instance, surface, device);
	}

	return physicalDevices;
}

uint32_t PhysicalDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format!");
}

bool PhysicalDevice::isSuitable() const {
	QueueFamilyIndices indices = findQueueFamilies();

	bool extensionsSupported = supportsDeviceExtensions();

	bool swapchainAdequate = false;
	if (extensionsSupported) {
		SwapchainSupportDetails swapchainSupport = querySwapchainSupport();
		swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies() const {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

bool PhysicalDevice::supportsDeviceExtensions() const {
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(std::string(extension.extensionName));
	}

	return requiredExtensions.empty();
}

SwapchainSupportDetails PhysicalDevice::querySwapchainSupport() const {
	SwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

}