#version 450

layout(location = 0) in vec2 sOffset;

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

layout(push_constant) uniform Push {
	vec4 position;
	vec4 color;
	float radius;
} push;

const float PI = 3.14159265359;

void main() {
	float distSquared = dot(sOffset, sOffset);

	if (distSquared > 1.0) {
		discard;
	}
	float cosTerm = 0.5 * (cos(distSquared * PI) + 1.0);

	outColor = vec4(push.color.rgb + cosTerm, cosTerm);
}