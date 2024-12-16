#include "GaussianSplatting/PointCloud.hpp"

#include "Core/Logger.hpp"
#include "Core/Asserts.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>

namespace stl {

PointCloud::PointCloud(Device& device, std::shared_ptr<Gaussians> gaussians)
	: m_Device{ device } {
	createVertexBuffers(*gaussians);
}

PointCloud::~PointCloud() {
}

void PointCloud::bind(VkCommandBuffer commandBuffer, uint32_t binding) {
	VkBuffer buffers[] = { m_VertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, binding, 1, buffers, offsets);
}

void PointCloud::draw(VkCommandBuffer commandBuffer) const {
	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}

void PointCloud::createVertexBuffers(const std::vector<RichPoint>& vertices) {
	m_VertexCount = static_cast<uint32_t>(vertices.size());

	SASSERT_MSG(m_VertexCount > 0, "Point cloud has to contain at least one point!");

	uint32_t vertexSize = sizeof(vertices[0]);
	VkDeviceSize bufferSize = vertexSize * m_VertexCount;

	Buffer stagingBuffer{ m_Device,
		vertexSize,
		m_VertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)vertices.data());

	m_VertexBuffer = std::make_unique<Buffer>(m_Device,
		vertexSize,
		m_VertexCount,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_Device.copyBuffer(stagingBuffer.getBuffer(), m_VertexBuffer->getBuffer(), bufferSize);
}

}
