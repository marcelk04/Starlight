#pragma once

#include "GaussianSplatting/SplatData.hpp"
#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/Buffer.hpp"

#include <memory>
#include <vector>
#include <string>

namespace stl {

class PointCloud {
public:
	PointCloud(Device& device, std::shared_ptr<Splats> splats);
	~PointCloud();

	PointCloud(const PointCloud&) = delete;
	PointCloud& operator=(const PointCloud&) = delete;

	uint32_t getCount() const { return m_VertexCount; }

	void bind(VkCommandBuffer commandBuffer, uint32_t binding = 0);
	void draw(VkCommandBuffer commandBuffer) const;

	static std::shared_ptr<Splats> loadFromSplatsPly(const std::string& path);

private:
	void createVertexBuffers(const std::vector<RichPoint>& vertices);

private:
	Device& m_Device;

	std::unique_ptr<Buffer> m_VertexBuffer;
	uint32_t m_VertexCount;
};

}
