#pragma once

#include "Window.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Model.hpp"

#include <vector>
#include <memory>

namespace stl {

class Renderer {
public:
	Renderer(Window& window, Device& device);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	VkRenderPass getSwapchainRenderPass() const { return m_Swapchain->getRenderPass(); }
	float getAspectRatio() const { return m_Swapchain->extentAspectRatio(); }
	bool isFrameInProgress() const { return m_IsFrameStarted; }
	VkCommandBuffer getCurrentCommandBuffer() const;
	int getFrameIndex() const;

	VkCommandBuffer beginFrame();
	void endFrame();

	void beginSwapchainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapchainRenderPass(VkCommandBuffer commandBuffer);

private:
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapchain();

private:
	Window& m_Window;
	Device& m_Device;

	std::unique_ptr<Swapchain> m_Swapchain;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	uint32_t m_CurrentImageIndex{ 0 };
	int m_CurrentFrameIndex{ 0 };
	bool m_IsFrameStarted{ false };
};

}