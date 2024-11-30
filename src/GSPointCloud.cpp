#include "GaussianSplatting/GSPointCloud.hpp"

#include "Core/Logger.hpp"
#include "Core/Asserts.hpp"

#include <fstream>
#include <filesystem>
#include <sstream>

namespace stl {

GSPointCloud::GSPointCloud(Device& device, std::shared_ptr<GSSplats> splats)
	: m_Device{ device } {
	createVertexBuffers(splats->splats);
}

GSPointCloud::~GSPointCloud() {
}

void GSPointCloud::bind(VkCommandBuffer commandBuffer, uint32_t binding) {
	VkBuffer buffers[] = { m_VertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, binding, 1, buffers, offsets);
}

void GSPointCloud::draw(VkCommandBuffer commandBuffer) const {
	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}

std::shared_ptr<GSSplats> GSPointCloud::loadFromSplatsPly(const std::string& path) {
	std::shared_ptr<GSSplats> splats = std::make_unique<GSSplats>();

	splats->numSplats = 0;
	splats->valid = false;

	std::ifstream reader(std::filesystem::absolute(path), std::ios::binary);
	if (!reader.good()) {
		SERROR("Bad PLY reader: ", path);
		return std::move(splats);
	}

	std::string buf;
	std::getline(reader, buf);
	std::getline(reader, buf);
	std::getline(reader, buf);
	std::stringstream ss(buf);
	std::string dummy;

	ss >> dummy >> dummy >> splats->numSplats;
	splats->splats.resize(splats->numSplats);

	SINFO("Loading ", splats->numSplats, " splats...");

	while (std::getline(reader, dummy)) {
		if (dummy.compare("end_header") == 0) {
			break;
		}
	}

	reader.read((char*)splats->splats.data(), splats->numSplats * sizeof(RichPoint));
	if (reader.eof()) {
		SERROR("Reader is EOF?");
		return std::move(splats);
	}

	splats->valid = true;

	return std::move(splats);
}

void GSPointCloud::createVertexBuffers(const std::vector<RichPoint>& vertices) {
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
