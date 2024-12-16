#include "GaussianSplatting/Ellipsoids.hpp"

#include "Core/Asserts.hpp"
#include "Core/Common.hpp"

#include <vector>
#include <array>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace stl {

Ellipsoids::Ellipsoids(Device& device, std::shared_ptr<std::vector<RichPoint>> gaussians)
	: m_Device{ device } {
	createBuffers(*gaussians);
}

Ellipsoids::~Ellipsoids() {
}

void Ellipsoids::bind(VkCommandBuffer commandBuffer, uint32_t binding) {
	VkBuffer buffers[] = { m_PositionBuffer->getBuffer(), m_ScaleBuffer->getBuffer(), m_SHBuffer->getBuffer(), m_QuaternionBuffer->getBuffer(), m_AlphaBuffer->getBuffer() };
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
	std::vector<glm::vec4> positions;
	std::vector<glm::vec4> scales;
	std::vector<std::array<glm::vec3, 16>> shs;
	std::vector<glm::vec4> quaternions;
	std::vector<float> alphas;

	positions.reserve(points.size());
	scales.reserve(points.size());
	shs.reserve(points.size());
	quaternions.reserve(points.size());
	alphas.reserve(points.size());

	const float alphaCutoff = 0.3f;
	const float eps = 1e-3;

	for (const RichPoint& point : points) {
		float alpha = Common::sigmoid(point.opacity);
		if (alpha < alphaCutoff) continue;

		positions.emplace_back(point.position, 1.0f);

		glm::vec3 scale = glm::exp(point.scale);
		scales.emplace_back(scale, 1.0f);

		std::array<glm::vec3, 16> sh;
		std::memcpy(sh.data(), point.shs.shs, 48);
		shs.emplace_back(std::move(sh));

		glm::vec4 rotation = glm::normalize(point.rotation);
		quaternions.emplace_back(rotation);

		alphas.emplace_back(alpha);
	}

	positions.shrink_to_fit();
	scales.shrink_to_fit();
	shs.shrink_to_fit();
	quaternions.shrink_to_fit();
	alphas.shrink_to_fit();

	m_PointCount = positions.size();

	SASSERT_MSG(m_PointCount > 0, "Point cloud has to contain at least one gaussian!");

	createBuffer(sizeof(positions[0]),   positions.size(),   (void*)positions.data(),   m_PositionBuffer);
	createBuffer(sizeof(scales[0]),      scales.size(),      (void*)scales.data(),      m_ScaleBuffer);
	createBuffer(sizeof(shs[0]),         shs.size(),         (void*)shs.data(),         m_SHBuffer);
	createBuffer(sizeof(quaternions[0]), quaternions.size(), (void*)quaternions.data(), m_QuaternionBuffer);
	createBuffer(sizeof(alphas[0]),      alphas.size(),      (void*)alphas.data(),      m_AlphaBuffer);

	SINFO("Loaded ", positions.size(), " gaussians into GPU memory");
}

}
