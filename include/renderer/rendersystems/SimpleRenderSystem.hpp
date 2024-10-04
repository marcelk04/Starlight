#pragma once

#include "renderer/wrapper/Pipeline.hpp"
#include "renderer/wrapper/Device.hpp"
#include "renderer/Model.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <memory>

namespace stl {

struct SimplePushConstantData {
	glm::mat4 transform{ 1.0f };
	glm::mat4 normalMatrix{ 1.0f };
};

class SimpleRenderSystem {
public:
	SimpleRenderSystem(Device& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera);

private:
	void createPipelineLayout();
	void createPipeline(VkRenderPass renderPass);

private:
	Device& m_Device;

	std::unique_ptr<Pipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
};

}