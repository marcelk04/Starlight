#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 4) in vec3 instancePos;
layout(location = 5) in vec3 instanceNormal;
layout(location = 6) in vec3 instanceSH;
layout(location = 7) in float instanceOpacity;
layout(location = 8) in vec3 instanceScale;
layout(location = 9) in vec4 instanceRotation;

layout(location = 0) out vec3 sColor;

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

void main() {
	sColor = instanceSH * 0.28 + vec3(0.5, 0.5, 0.5);

	gl_Position = ubo.projection * ubo.view * vec4((inPos * 0.01) + instancePos, 1.0);
	gl_PointSize = 2.0;
}