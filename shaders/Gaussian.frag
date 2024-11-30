#version 450

layout(location = 0) in vec3 sColor;
layout(location = 1) in float sAlpha;

layout(location = 0) out vec4 outColor;

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
	outColor = vec4(sColor, sAlpha);
}