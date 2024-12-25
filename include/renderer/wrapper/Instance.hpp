#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace stl {

class Instance {
public:
	Instance();
	Instance(const Instance&) = delete;
	Instance(Instance&& other) noexcept;
	~Instance();

	Instance& operator=(const Instance&) = delete;
	Instance& operator=(Instance&& other) noexcept;

	VkInstance getInstance() const { return m_Instance; }

private:
	void createInstance();
	void setupDebugMessenger();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
	std::vector<const char*> getRequiredExtensions() const;
	bool supportsValidationLayers() const;
	bool supportsRequiredExtensions() const;

	static bool supportsLayers(const std::vector<const char*>& layers);
	static bool supportsExtensions(const std::vector<const char*>& extensions);

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