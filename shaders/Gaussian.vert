#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inSH;
layout(location = 3) in float inOpacity;
layout(location = 4) in vec3 inScale;
layout(location = 5) in vec4 inRotation;

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
	sColor = inSH * 0.28 + vec3(0.5, 0.5, 0.5);

	gl_Position = ubo.projection * ubo.view * vec4(inPosition, 1.0);
	gl_PointSize = 2.0;
}