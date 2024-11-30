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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(6);
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(RichPoint, position);

		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(RichPoint, normal);

		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(RichPoint, shs);

		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].format = VK_FORMAT_R32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(RichPoint, opacity);

		attributeDescriptions[4].location = 4;
		attributeDescriptions[4].binding = 0;
		attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[4].offset = offsetof(RichPoint, scale);

		attributeDescriptions[5].location = 5;
		attributeDescriptions[5].binding = 0;
		attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[5].offset = offsetof(RichPoint, rotation);

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

struct Splats {
	bool valid;
	int numSplats;
	std::vector<RichPoint> splats;
};

}
