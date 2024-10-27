#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace stl {

class Instance {
public:
	Instance();
	~Instance();

	VkInstance getInstance() const { return m_Instance; }

private:
	void createInstance();
	void setupDebugMessenger();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
	std::vector<const char*> getRequiredExtensions() const;
	bool supportsValidationLayers() const;
	bool supportsRequiredExtensions() const;

private:
	VkInstance m_Instance;
	VkDebugUtilsMessengerEXT m_DebugMessenger;

#ifdef NDEBUG
	bool m_EnableValidationLayers = false;
#else
	bool m_EnableValidationLayers = true;
#endif

	const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
};

}