#pragma once

#include "GaussianSplatting/SplatData.hpp"
#include "renderer/wrapper/Device.hpp"
#include "renderer/wrapper/Buffer.hpp"

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace stl {

class Ellipsoids {
public:
	Ellipsoids(Device& device, std::shared_ptr<std::vector<RichPoint>> gaussians);
	~Ellipsoids();

	Ellipsoids(const Ellipsoids&) = delete;
	Ellipsoids& operator=(const Ellipsoids&) = delete;

	uint32_t getCount() const { return m_PointCount; }

	void bind(VkCommandBuffer commandBuffer, uint32_t binding = 0);
	void draw(VkCommandBuffer commandBuffer) const;

private:
	void createBuffer(uint32_t elementSize, uint32_t elementCount, void* data, std::unique_ptr<Buffer>& buffer);
	void createBuffers(const std::vector<RichPoint>& points);

private:
	Device& m_Device;

	uint32_t m_PointCount;

	std::unique_ptr<Buffer> m_PositionBuffer;
	std::unique_ptr<Buffer> m_ScaleBuffer;
	std::unique_ptr<Buffer> m_SHBuffer;
	std::unique_ptr<Buffer> m_QuaternionBuffer;
	std::unique_ptr<Buffer> m_AlphaBuffer;
};

}
