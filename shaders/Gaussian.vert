#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 4) in vec4 instancePos;
layout(location = 5) in vec4 instanceScale;
layout(location = 6) in vec4 instanceColor;
layout(location = 7) in vec4 instanceQuat;
layout(location = 8) in float instanceAlpha;

layout(location = 0) out vec3 sColor;
layout(location = 1) out vec3 sPosition;
layout(location = 2) out vec3 sCenter;
layout(location = 3) out vec3 sScale;
layout(location = 4) out mat3 sRot;
layout(location = 7) out float sAlpha;

struct PointLight {
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor;
	PointLight pointLights[10];
	int numLights;
} ubo;

mat3 quatToMat(vec4 q) {
    return mat3(2.0 * (q.x * q.x + q.y * q.y) - 1.0, 2.0 * (q.y * q.z + q.x * q.w), 2.0 * (q.y * q.w - q.x * q.z), // 1st column
                2.0 * (q.y * q.z - q.x * q.w), 2.0 * (q.x * q.x + q.z * q.z) - 1.0, 2.0 * (q.z * q.w + q.x * q.y), // 2nd column
                2.0 * (q.y * q.w + q.x * q.z), 2.0 * (q.z * q.w - q.x * q.y), 2.0 * (q.x * q.x + q.w * q.w) - 1.0); // last column
}

void main() {
	vec3 scale = instanceScale.xyz * 2.0;
	vec3 scaled = scale * (inPos);

	mat3 rot = quatToMat(instanceQuat);
	vec3 rotated = rot * scaled;

	vec3 posOffset = rotated + instancePos.xyz;
	vec4 mPos = vec4(posOffset, 1.0);

	sColor = instanceColor.xyz * 0.28 + vec3(0.5, 0.5, 0.5);
	sPosition = posOffset;
	sCenter = instancePos.xyz;
	sScale = scale;
	sRot = rot;
	sAlpha = instanceAlpha;

	gl_Position = ubo.projection * ubo.view * mPos;
}