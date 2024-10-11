#version 450

layout(location = 0) in vec3 sColor;
layout(location = 1) in vec3 sWorldPos;
layout(location = 2) in vec3 sNormal;

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
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec3 diffuseLight = ubo.ambientLightColor.rgb * ubo.ambientLightColor.a;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(sNormal);

	vec3 worldCamPos = ubo.inverseView[3].xyz;
	vec3 viewDirection = normalize(worldCamPos - sWorldPos.xyz);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];

		vec3 directionToLight = light.position.xyz - sWorldPos.xyz;
		float attenuation = 1.0 / dot(directionToLight, directionToLight);

		directionToLight = normalize(directionToLight);

		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		diffuseLight += intensity * max(dot(surfaceNormal, directionToLight), 0);

		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 32);
		specularLight += intensity * blinnTerm;
	}

	outColor = vec4((diffuseLight + specularLight) * sColor, 1.0);
}