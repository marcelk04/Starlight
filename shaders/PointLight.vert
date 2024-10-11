#version 450

layout(location = 0) out vec2 sOffset;

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

const vec2 OFFSETS[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0,  1.0),
	vec2( 1.0, -1.0),
	vec2( 1.0, -1.0),
	vec2(-1.0,  1.0),
	vec2( 1.0,  1.0)
);

const float LIGHT_RADIUS = 0.1;

void main() {
	sOffset = OFFSETS[gl_VertexIndex];

	vec4 camSpaceLightPos = ubo.view * vec4(push.position.xyz, 1.0);
	vec4 camSpacePos = camSpaceLightPos + push.radius * vec4(sOffset, 0.0, 0.0);

	gl_Position = ubo.projection * camSpacePos;
}