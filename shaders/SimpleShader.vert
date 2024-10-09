#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 sColor;
layout(location = 1) out vec3 sWorldPos;
layout(location = 2) out vec3 sNormal;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec4 worldPosition = push.modelMatrix * vec4(inPosition, 1.0);

	gl_Position = ubo.projection * ubo.view * worldPosition;

	sColor = inColor;
	sWorldPos = worldPosition.xyz;
	sNormal = normalize(mat3(push.normalMatrix) * inNormal);
}