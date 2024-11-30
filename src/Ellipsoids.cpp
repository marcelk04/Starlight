#include "GaussianSplatting/Ellipsoids.hpp"

#include "Core/Asserts.hpp"
#include "Core/Common.hpp"

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace stl {

Ellipsoids::Ellipsoids(Device& device, std::shared_ptr<Splats> splats)
	: m_Device{ device } {
	createBuffers(splats->splats);
}

Ellipsoids::~Ellipsoids() {
}

void Ellipsoids::bind(VkCommandBuffer commandBuffer, uint32_t binding) {
	VkBuffer buffers[] = { m_PositionBuffer->getBuffer(), m_ScaleBuffer->getBuffer(), m_ColorBuffer->getBuffer(), m_QuaternionBuffer->getBuffer(), m_AlphaBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0, 0, 0, 0, 0 };
	vkCmdBindVertexBuffers(commandBuffer, binding, 5, buffers, offsets);
}

void Ellipsoids::draw(VkCommandBuffer commandBuffer) const {
	vkCmdDraw(commandBuffer, m_PointCount, 1, 0, 0);
}

void Ellipsoids::createBuffer(uint32_t elementSize, uint32_t elementCount, void* data, std::unique_ptr<Buffer>& buffer) {
	VkDeviceSize bufferSize = elementSize * elementCount;

	Buffer stagingBuffer{ m_Device,
		elementSize,
		elementCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

	stagingBuffer.map();
	stagingBuffer.writeToBuffer(data);

	buffer = std::make_unique<Buffer>(m_Device,
		elementSize,
		elementCount,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_Device.copyBuffer(stagingBuffer.getBuffer(), buffer->getBuffer(), bufferSize);
}

void Ellipsoids::createBuffers(const std::vector<RichPoint>& points) {
	m_PointCount = static_cast<uint32_t>(points.size());

	SASSERT_MSG(m_PointCount > 0, "Point cloud has to contain at least one point!");

	std::vector<glm::vec4> positions(points.size());
	std::vector<glm::vec4> scales(points.size());
	std::vector<glm::vec4> colors(points.size());
	std::vector<glm::vec4> quaternions(points.size());
	std::vector<float> alphas(points.size());

	for (int i = 0; i < points.size(); i++) {
		const glm::vec3& pos = points[i].position;
		positions[i] = glm::vec4(pos.x, pos.y, pos.z, 1.0f);

		const glm::vec3& scale = points[i].scale;
		scales[i] = glm::vec4(exp(scale.x), exp(scale.y), exp(scale.z), 1.0f);

		const SHs<3>& shs = points[i].shs;
		colors[i] = glm::vec4(shs.shs[0], shs.shs[1], shs.shs[2], 1.0f);

		quaternions[i] = glm::normalize(points[i].rotation);

		alphas[i] = Common::sigmoid(points[i].opacity);
	}

	createBuffer(sizeof(positions[0]), positions.size(), (void*)positions.data(), m_PositionBuffer);
	createBuffer(sizeof(scales[0]), scales.size(), (void*)scales.data(), m_ScaleBuffer);
	createBuffer(sizeof(colors[0]), colors.size(), (void*)colors.data(), m_ColorBuffer);
	createBuffer(sizeof(quaternions[0]), quaternions.size(), (void*)quaternions.data(), m_QuaternionBuffer);
	createBuffer(sizeof(alphas[0]), alphas.size(), (void*)alphas.data(), m_AlphaBuffer);
}

}
