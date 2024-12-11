#version 450

layout(location = 0) in vec3 sColor;
layout(location = 1) in vec3 sPosition;
layout(location = 2) in vec3 sCenter;
layout(location = 3) in vec3 sScale;
layout(location = 4) in mat3 sRot;
layout(location = 7) in float sAlpha;

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

bool ellipsoidIntersectWithOutput(vec3 rayOrigin, vec3 rayDir, out vec3 intersection, out vec3 normal) {
	dvec3 localRayOrigin = (rayOrigin - sCenter) * sRot;
	dvec3 localRayDir = normalize(rayDir * sRot);

	dvec3 invScale = 1.0 / dvec3(sScale);

	double a = dot(localRayDir * invScale, localRayDir * invScale);
	double b = 2.0 * dot(localRayDir * invScale, localRayOrigin * invScale);
	double c = dot(localRayOrigin * invScale, localRayOrigin * invScale) - 1.0;

	double discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0) {
		return false;
	}

	float t1 = float((-b - sqrt(discriminant)) / (2.0 * a));
	float t2 = float((-b + sqrt(discriminant)) / (2.0 * a));

	float t = min(t1, t2);

	vec3 localIntersection = vec3(localRayOrigin + t * localRayDir);
	vec3 localNormal = normalize(localIntersection * vec3(invScale));

	intersection = sRot * localIntersection + sCenter;
	normal = normalize(sRot * localNormal);

	return true;
}

bool ellipsoidIntersect(vec3 rayOrigin, vec3 rayDir) {
	dvec3 invScale = 1.0 / dvec3(sScale);

	dvec3 localRayOrigin = ((rayOrigin - sCenter) * sRot) * invScale;
	dvec3 localRayDir = normalize(rayDir * sRot) * invScale;

	double a = dot(localRayDir, localRayDir);
	double b = 2.0 * dot(localRayDir, localRayOrigin);
	double c = dot(localRayOrigin, localRayOrigin) - 1.0;

	double discriminant = b * b - 4.0 * a * c;

	return discriminant >= 0.0;
}

void main() {
    vec3 camPos = ubo.inverseView[3].xyz;
	//vec3 dir = normalize(sPosition - camPos);
	vec3 dir = sPosition - camPos;

	vec3 intersection = vec3(0.0);
	vec3 normal = vec3(0.0);

	if (!ellipsoidIntersect(camPos, dir)) {
		discard;
	}

	/*
	float align = max(0.1, dot(-dir, normal));

	vec4 newPos = ubo.projection * ubo.view * vec4(intersection, 1.0);
	newPos /= newPos.w;

	gl_FragDepth = newPos.z;
	*/

	outColor = vec4(sColor, sAlpha);
}