#include "Renderer.hpp"

#include <array>
#include <stdexcept>
#include <cassert>

namespace stl {

Renderer::Renderer(Window& window, Device& device)
	: m_Window{ window }, m_Device{ device } {
	recreateSwapchain();
	createCommandBuffers();
}

Renderer::~Renderer() {
	freeCommandBuffers();
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const {
	assert(m_IsFrameStarted && "Cannont get command buffer when frame is not in progress");
	return m_CommandBuffers[m_CurrentFrameIndex];
}

int Renderer::getFrameIndex() const {
	assert(m_IsFrameStarted && "Cannot get frame index when frame is not in progress!");
	return m_CurrentFrameIndex;
}

VkCommandBuffer Renderer::beginFrame() {
	assert(!m_IsFrameStarted && "Cannot call beginFrame while already in progress");

	VkResult result = m_Swapchain->acquireNextImage(&m_CurrentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return VK_NULL_HANDLE;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swapchain image!");
	}

	m_IsFrameStarted = true;

	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	return commandBuffer;
}

void Renderer::endFrame() {
	assert(m_IsFrameStarted && "Cannot call endFrame while frame is not in progress!");

	VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}

	VkResult result = m_Swapchain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
		m_Window.resetWindowResizedFlag();
		recreateSwapchain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swapchain image!");
	}

	m_IsFrameStarted = false;
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Swapchain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapchainRenderPass(VkCommandBuffer commandBuffer) {
	assert(m_IsFrameStarted && "Cannot call beginSwapchainRenderPass if frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame!");

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_Swapchain->getRenderPass();
	renderPassInfo.framebuffer = m_Swapchain->getFramebuffer(m_CurrentImageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_Swapchain->getSwapchainExtent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_Swapchain->getSwapchainExtent().width);
	viewport.height = static_cast<float>(m_Swapchain->getSwapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Swapchain->getSwapchainExtent();

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapchainRenderPass(VkCommandBuffer commandBuffer) {
	assert(m_IsFrameStarted && "Cannot call endSwapchainRenderPass if frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame!");

	vkCmdEndRenderPass(commandBuffer);
}

void Renderer::createCommandBuffers() {
	m_CommandBuffers.resize(Swapchain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.getDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}

void Renderer::freeCommandBuffers() {
	vkFreeCommandBuffers(m_Device.getDevice(), m_Device.getCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

void Renderer::recreateSwapchain() {
	VkExtent2D extent = m_Window.getExtent();

	while (extent.width == 0 || extent.height == 0) {
		extent = m_Window.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_Device.getDevice());

	if (m_Swapchain == nullptr) {
		m_Swapchain = std::make_unique<Swapchain>(m_Device, extent);
	} else {
		std::shared_ptr<Swapchain> oldSwapchain = std::move(m_Swapchain);

		m_Swapchain = std::make_unique<Swapchain>(m_Device, extent, oldSwapchain);

		if (!oldSwapchain->compareSwapFormats(*m_Swapchain)) {
			throw std::runtime_error("Swapchain image or depth format has changed!");
		}
	}
}

}