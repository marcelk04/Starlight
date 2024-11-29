#pragma once

#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace stl {

template<int D>
struct SHs {
	float shs[(D + 1) * (D + 1) * 3];
};

struct RichPoint {
	glm::vec3 position;
	glm::vec3 normal;
	SHs<3> shs;
	glm::vec3 scale;
	glm::vec4 rotation;
	float opacity;

	static std::vector<VkVertexInputBindingDescription> getBindingDescription() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(RichPoint);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescription() {
		size_t shSize = 12;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(5 + shSize);
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(RichPoint, position);

		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(RichPoint, normal);

		for (uint32_t i = 0; i < shSize; i++) {
			attributeDescriptions[2 + i].location = 2 + i;
			attributeDescriptions[2 + i].binding = 0;
			attributeDescriptions[2 + i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[2 + i].offset = offsetof(RichPoint, shs) + 4 * i;
		}

		attributeDescriptions[shSize + 2].location = shSize + 2;
		attributeDescriptions[shSize + 2].binding = 0;
		attributeDescriptions[shSize + 2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[shSize + 2].offset = offsetof(RichPoint, scale);

		attributeDescriptions[shSize + 3].location = shSize + 3;
		attributeDescriptions[shSize + 3].binding = 0;
		attributeDescriptions[shSize + 3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[shSize + 3].offset = offsetof(RichPoint, rotation);

		attributeDescriptions[shSize + 4].location = shSize + 4;
		attributeDescriptions[shSize + 4].binding = 0;
		attributeDescriptions[shSize + 4].format = VK_FORMAT_R32_SFLOAT;
		attributeDescriptions[shSize + 4].offset = offsetof(RichPoint, opacity);

		return attributeDescriptions;
	}

	bool operator==(const RichPoint& other) const {
		return position == other.position
			&& normal == other.normal
			&& opacity == other.opacity
			&& scale == other.scale
			&& rotation == other.rotation;
	}
};

struct GSSplats {
	bool valid;
	int numSplats;
	std::vector<RichPoint> splats;
};

}
