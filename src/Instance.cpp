#include "renderer/wrapper/Instance.hpp"

#include "Core/Logger.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <set>
#include <utility>
#include <string>

namespace stl {

// local callback functions

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	switch (messageSeverity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		SERROR(pCallbackData->pMessage);
		return VK_FALSE;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		SWARN(pCallbackData->pMessage);
		return VK_FALSE;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		SINFO(pCallbackData->pMessage);
		return VK_FALSE;

	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		SDEBUG(pCallbackData->pMessage);
		return VK_FALSE;

	default:
		return VK_FALSE;
	}
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

// class member functions

/**
 * Creates a new Vulkan instance.
 */
Instance::Instance() {
	createInstance();
	setupDebugMessenger();
}

Instance::Instance(Instance&& other) noexcept
	: m_Instance{ std::exchange(other.m_Instance, VK_NULL_HANDLE) },
	m_DebugMessenger{ std::exchange(other.m_DebugMessenger, VK_NULL_HANDLE) },
	m_EnableValidationLayers{ std::exchange(other.m_EnableValidationLayers, false) } {
}

Instance::~Instance() {
	if (m_EnableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}

	vkDestroyInstance(m_Instance, nullptr);
}

/*
 * Function used internally to create a Vulkan instance.
 * If enabled and present, validation layers are also added.
 * Also checks if the required extensions are present.
 */
void Instance::createInstance() {
	if (m_EnableValidationLayers && !supportsValidationLayers()) {
		SWARN("Validation layers requested, but not available!");
		SWARN("Disabling validation layers!");

		m_EnableValidationLayers = false;
	}

	if (!supportsRequiredExtensions()) {
		throw std::runtime_error("Missing required extensions!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Starlight Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};

	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (m_EnableValidationLayers) {
		SINFO("Validation layers are enabled");

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	} else {
		SINFO("Validation layers are disabled");

		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

/**
 * Sets up a debug messenger.
 */
void Instance::setupDebugMessenger() {
	if (!m_EnableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup debug messenger!");
	}
}

/**
 * Populates the create info for a debug messenger with a default configuration.
 *
 * @param[in,out] createInfo the create info to be filled.
 */
void Instance::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

/**
 * Returns the extensions required by the engine.
 * This includes the glfw extensions and the debug utils extension.
 *
 * @return a vector of the names of the required extensions.
 */
std::vector<const char*> Instance::getRequiredExtensions() const {
	uint32_t glfwExtensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_EnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

/**
 * Checks if all validation layers specified in m_ValidationLayers are present.
 *
 * @return true, if all validation layers are found.
 */
bool Instance::supportsValidationLayers() const {
	return supportsLayers(m_ValidationLayers);
}

/**
 * Checks if all required extensions are present.
 *
 * @see getRequiredExtensions()
 *
 * @return true, if all extensions are found.
 */
bool Instance::supportsRequiredExtensions() const {
	return supportsExtensions(getRequiredExtensions());
}

bool Instance::supportsLayers(const std::vector<const char*>& layers) {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::set<std::string> requiredLayers(layers.begin(), layers.end());

	for (const auto& layer : availableLayers) {
		requiredLayers.erase(std::string(layer.layerName));
	}

	for (const auto& layer : requiredLayers) {
		SWARN("Missing layer: ", layer);
	}

	return requiredLayers.empty();
}

bool Instance::supportsExtensions(const std::vector<const char*>& extensions) {
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(std::string(extension.extensionName));
	}

	for (const auto& extension : requiredExtensions) {
		SWARN("Missing extension: ", extension);
	}

	return requiredExtensions.empty();
}

}