#pragma once

#include "renderer/wrapper/Device.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <memory>

namespace stl {

class Swapchain {
public:
	Swapchain(Device& device, VkExtent2D windowExtent);
	Swapchain(Device& device, VkExtent2D windowExtent, std::shared_ptr<Swapchain> previousSwapchain);
	~Swapchain();

	Swapchain(const Swapchain&) = delete;
	Swapchain& operator=(const Swapchain&) = delete;

	VkFramebuffer getFramebuffer(int index) const { return m_SwapchainFramebuffers[index]; }
	VkRenderPass getRenderPass() const { return m_RenderPass; }
	VkImageView getImageView(int index) const { return m_SwapchainImageViews[index]; }
	size_t imageCount() const { return m_SwapchainImages.size(); }
	VkFormat getSwapchainImageFormat() const { return m_SwapchainImageFormat; }
	VkExtent2D getSwapchainExtent() const { return m_SwapchainExtent; }
	uint32_t getWidth() const { return m_SwapchainExtent.width; }
	uint32_t getHeight() const { return m_SwapchainExtent.height; }

	float extentAspectRatio() const;
	VkFormat findDepthFormat() const;
	VkResult acquireNextImage(uint32_t* imageIndex) const;
	VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	bool compareSwapFormats(const Swapchain& swapchain) const;

private:
	void init();
	void createSwapchain();
	void createImageViews();
	void createDepthResources();
	void createRenderPass();
	void createFramebuffers();
	void createSyncObjects();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

public:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

private:
	VkFormat m_SwapchainImageFormat;
	VkFormat m_SwapchainDepthFormat;
	VkExtent2D m_SwapchainExtent;

	std::vector<VkFramebuffer> m_SwapchainFramebuffers;
	VkRenderPass m_RenderPass;

	std::vector<VkImage> m_DepthImages;
	std::vector<VkDeviceMemory> m_DepthImageMemories;
	std::vector<VkImageView> m_DepthImageViews;
	std::vector<VkImage> m_SwapchainImages;
	std::vector<VkImageView> m_SwapchainImageViews;

	Device& m_Device;
	VkExtent2D m_WindowExtent;

	VkSwapchainKHR m_Swapchain;
	std::shared_ptr<Swapchain> m_OldSwapchain;

	std::vector<VkSemaphore> m_ImageAvailableSemaphores;
	std::vector<VkSemaphore> m_RenderFinishedSemaphores;
	std::vector<VkFence> m_InFlightFences;
	std::vector<VkFence> m_ImagesInFlight;
	size_t m_CurrentFrame = 0;
};

}