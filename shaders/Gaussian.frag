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

/**
 * This function checks for whether (p - ro) intersects a sphere
 * located at c, with radius r = 1.0.
 */
vec3 sphereIntersect(vec3 c, vec3 ro, vec3 p, out vec3 normal) {
    mat3 sphereRotationT = transpose(sRot);

    vec3 rd = vec3(sphereRotationT * normalize(p - ro)) / vec3(sScale);
    vec3 u = (sphereRotationT * vec3(ro - c)) / vec3(sScale); // ro relative to c

    float a = dot(rd, rd);
    float b = 2.0 * dot(u, rd);
    float cc = dot(u, u) - 1.0;

    float discriminant = b * b - 4 * a * cc;

    // no intersection
    if (discriminant < 0.0) {
        return vec3(0.0);
    }

    float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
    float t2 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t = min(t1, t2);
    vec3 intersection = ro + sRot * (vec3(t * rd) * sScale);
    vec3 localIntersection = ((mat3(sphereRotationT) * (intersection - c)) / sScale);

    normal = sRot * localIntersection;
    return intersection;
}

void main() {
	if (sAlpha < 0.3) {
		discard;
	}

    vec3 camPos = vec3(ubo.inverseView[3]);
    vec3 normal = vec3(0.0);

    vec3 intersection = sphereIntersect(sCenter, camPos, sPosition, normal);

    if (intersection == vec3(0.0)) {
        discard;
    }

    vec3 rd = normalize(camPos - intersection);
    float align = max(dot(rd, normal), 0.1);

    vec4 newPos = ubo.projection * ubo.view * vec4(intersection, 1.0);
    newPos /= newPos.w;
    gl_FragDepth = newPos.z;

	outColor = vec4(align * sColor, 1.0);
}