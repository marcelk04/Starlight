#version 450

layout(location = 0) in vec3 sColor;
layout(location = 1) in vec3 sWorldPos;
layout(location = 2) in vec3 sNormal;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec4 ambientLightColor;
	vec3 lightPosition;
	vec4 lightColor;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 directionToLight = ubo.lightPosition - sWorldPos.xyz;
	float attenuation = 1.0 / dot(directionToLight, directionToLight);

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalize(sNormal), normalize(directionToLight)), 0);

	outColor = vec4((diffuseLight + ambientLight) * sColor, 1.0);
}