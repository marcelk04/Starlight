#pragma once

#include "renderer/wrapper/Device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace stl {

class Model {
public:
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	struct Data {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
	};

public:
	Model(Device& device, const Model::Data& data);
	~Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer) const;

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<uint32_t>& indices);

private:
	Device& m_Device;

	VkBuffer m_VertexBuffer;
	VkDeviceMemory m_VertexBufferMemory;
	uint32_t m_VertexCount;

	bool m_HasIndexBuffer{ false };

	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
	uint32_t m_IndexCount;
};

}