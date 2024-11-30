#pragma once

#include "GaussianSplatting/SplatData.hpp"
#include "GaussianSplatting/PointCloud.hpp"
#include "GaussianSplatting/Ellipsoids.hpp"
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

class GaussianSystem {
public:
	GaussianSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~GaussianSystem();

	GaussianSystem(const GaussianSystem&) = delete;
	GaussianSystem& operator=(const GaussianSystem&) = delete;

	void render(FrameInfo& frameInfo, Ellipsoids& ellipsoids);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	std::vector<VkVertexInputBindingDescription> getBindingDescription() const;
	std::vector<VkVertexInputAttributeDescription> getAttributeDescription() const;

private:
	Device& m_Device;

	std::unique_ptr<Pipeline> m_Pipeline;
	VkPipelineLayout m_PipelineLayout;

	std::shared_ptr<Model> m_CubeModel;
};

}