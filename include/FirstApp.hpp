#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"
#include "Device.hpp"
#include "Swapchain.hpp"
#include "Model.hpp"

#include <vector>
#include <memory>

namespace stl {

class FirstApp {
public:
	FirstApp();
	~FirstApp();

	FirstApp(const FirstApp&) = delete;
	FirstApp& operator=(const FirstApp&) = delete;

	void run();

private:
	void loadModels();
	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
	void freeCommandBuffers();
	void drawFrame();
	void recreateSwapchain();
	void recordCommandBuffer(int imageIndex);

public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

private:
	Window m_Window{ WIDTH, HEIGHT, "Starlight" };
	Device m_Device{ m_Window };
	std::unique_ptr<Swapchain> m_Swapchain;
	std::unique_ptr<Pipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
	std::vector<VkCommandBuffer> m_CommandBuffers;
	std::unique_ptr<Model> m_Model;
};

}