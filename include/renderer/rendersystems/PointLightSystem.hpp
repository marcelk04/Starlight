#pragma once

#include "renderer/wrapper/Pipeline.hpp"
#include "renderer/wrapper/Device.hpp"
#include "renderer/Model.hpp"
#include "renderer/FrameInfo.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>
#include <memory>

namespace stl {

class PointLightSystem {
public:
	PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~PointLightSystem();

	PointLightSystem(const PointLightSystem&) = delete;
	PointLightSystem& operator=(const PointLightSystem&) = delete;

	void render(FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

private:
	Device& m_Device;

	std::unique_ptr<Pipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;
};

}