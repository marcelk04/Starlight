#version 450

// Per-Vertex Data
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

// Per-Instance Data
layout(location = 4) in vec4 instancePos;
layout(location = 5) in vec4 instanceScale;
layout(location = 6) in vec4[12] sh;
layout(location = 18) in vec4 instanceQuat;
layout(location = 19) in float instanceAlpha;

layout(location = 0) out vec3 sColor;
layout(location = 1) out vec3 sPosition;
layout(location = 2) out vec3 sCenter;
layout(location = 3) out vec3 sScale;
layout(location = 4) out mat3 sRot;
layout(location = 7) out float sAlpha;

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
	int
	numLights;
} ubo;

const float SH_C0 = 0.28209479177387814f;
const float SH_C1 = 0.4886025119029199f;
const float SH_C2[] = {
1.0925484305920792f,
1.0925484305920792f,
0.31539156525252005f,
1.0925484305920792f,
0.5462742152960396f
};
const float SH_C3[] = {
0.5900435899266435f,
2.890611442640554f,
0.4570457994644658f,
0.3731763325901154f,
0.4570457994644658f,
1.445305721320277f,
0.5900435899266435f
};

mat3 quatToMat(vec4 q) {
    return mat3(2.0 * (q.x * q.x + q.y * q.y) - 1.0, 2.0 * (q.y * q.z + q.x * q.w), 2.0 * (q.y * q.w - q.x * q.z), // 1st column
                2.0 * (q.y * q.z - q.x * q.w), 2.0 * (q.x * q.x + q.z * q.z) - 1.0, 2.0 * (q.z * q.w + q.x * q.y), // 2nd column
                2.0 * (q.y * q.w + q.x * q.z), 2.0 * (q.z * q.w - q.x * q.y), 2.0 * (q.x * q.x + q.w * q.w) - 1.0); // last column
}

vec3 computeSH(vec3 center, vec3 camPos) {
	vec3 direction = normalize(center - camPos);
	direction = direction.zxy;
	float x = direction.x, y = direction.y, z = direction.z;

	vec3 c = SH_C0 * vec3(sh[0].x, sh[0].y, sh[0].z)
		- SH_C1 * vec3(sh[0].w, sh[1].x, sh[1].y) * y
		+ SH_C1 * vec3(sh[1].z, sh[1].w, sh[2].x) * z
		- SH_C1 * vec3(sh[2].y, sh[2].z, sh[2].w) * x;
		/*+ SH_C2[0] * vec3(sh[3].x, sh[3].y, sh[3].z) * x * y
		- SH_C2[1] * vec3(sh[3].w, sh[4].x, sh[4].y) * y * z
		+ SH_C2[2] * vec3(sh[4].z, sh[4].w, sh[5].x) * (3.0 * z * z - 1.0)
		- SH_C2[3] * vec3(sh[5].y, sh[5].z, sh[5].w) * x * z
		+ SH_C2[4] * vec3(sh[6].x, sh[6].y, sh[6].z) * (x * x - y * y)
		- SH_C3[0] * vec3(sh[6].w, sh[7].x, sh[7].y) * y * (3.0 * x * x - y * y)
		+ SH_C3[1] * vec3(sh[7].z, sh[7].w, sh[7].x) * x * y * z
		- SH_C3[2] * vec3(sh[8].y, sh[8].z, sh[8].w) * y * (5.0 * z * z - 1.0)
		+ SH_C3[3] * vec3(sh[9].x, sh[9].y, sh[9].z) * z * (5.0 * z * z - 3.0)
		- SH_C3[4] * vec3(sh[9].w, sh[10].x, sh[10].y) * x * (5.0 * z * z - 1.0)
		+ SH_C3[5] * vec3(sh[10].z, sh[10].w, sh[11].x) * z * (x * x - y * y)
		- SH_C3[6] * vec3(sh[11].y, sh[11].z, sh[11].w) * x * (x * x - 3.0 * y * y);



/*
	vec3 c = SH_C0 * getSHVec3(0);

    c -= SH_C1 * getSHVec3(1) * y;
    c += SH_C1 * getSHVec3(2) * z;
    c -= SH_C1 * getSHVec3(3) * x;

    c += SH_C2[0] * getSHVec3(4) * x * y;
    c += SH_C2[1] * getSHVec3(5) * y * z;
    c += SH_C2[2] * getSHVec3(6) * (2.0 * z * z - x * x - y * y);
    c += SH_C2[3] * getSHVec3(7) * z * x;
    c += SH_C2[4] * getSHVec3(8) * (x * x - y * y);

    c += SH_C3[0] * getSHVec3(9) * (3.0 * x * x - y * y) * y;
    c += SH_C3[1] * getSHVec3(10) * x * y * z;
    c += SH_C3[2] * getSHVec3(11) * (4.0 * z * z - x * x - y * y) * y;
    c += SH_C3[3] * getSHVec3(12) * z * (2.0 * z * z - 3.0 * x * x - 3.0 * y * y);
    c += SH_C3[4] * getSHVec3(13) * x * (4.0 * z * z - x * x - y * y);
    c += SH_C3[5] * getSHVec3(14) * (x * x - y * y) * z;
    c += SH_C3[6] * getSHVec3(15) * x * (x * x - 3.0 * y * y);*/

	//c += 0.5;

	if (c.x < 0.0) {
		c.x = 0.0;
	}

	return c + vec3(0.5, 0.5, 0.5);
}

void main() {
	vec3 scale = instanceScale.xyz * 2.0;
	vec3 scaled = scale * (inPos);

	mat3 rot = quatToMat(instanceQuat);
	vec3 rotated = rot * scaled;

	vec3 posOffset = rotated + instancePos.xyz;
	vec4 mPos = vec4(posOffset, 1.0);

	sPosition = posOffset;
	sCenter = instancePos.xyz;
	sScale = scale;
	sRot = rot;
	sAlpha = instanceAlpha;

	sColor = computeSH(sCenter, ubo.inverseView[3].xyz);

	gl_Position = ubo.projection * ubo.view * mPos;
}