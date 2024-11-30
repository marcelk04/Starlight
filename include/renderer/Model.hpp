#pragma once

#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/Buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace stl {

class Model {
public:
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		}
	};

	struct Data {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string& filepath);
	};

public:
	Model(Device& device, const Model::Data& data);
	~Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filepath);

	void bind(VkCommandBuffer commandBuffer, uint32_t binding = 0);
	void draw(VkCommandBuffer commandBuffer, uint32_t instanceCount = 1) const;

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<uint32_t>& indices);

private:
	Device& m_Device;

	std::unique_ptr<Buffer> m_VertexBuffer;
	uint32_t m_VertexCount;

	bool m_HasIndexBuffer{ false };

	std::unique_ptr<Buffer> m_IndexBuffer;
	uint32_t m_IndexCount;
};

}