#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 0) out vec3 WorldPosition;

void main()
{
	WorldPosition = a_Position;
	gl_Position = vec4(a_Position, 1.0f);
}


#type fragment
#version 410 core

#include "../builtin/Math.glsl"
#include "IBLCommon.glsl"

layout(location = 0) in vec3 WorldPosition;
layout(location = 0) out vec4 OutColor;

layout(binding = 0) uniform sampler2D u_EquirectangularMap;
layout(location = 0) uniform int u_FaceId;

// 将方向映射到柱面环境贴图 UV, 水平方向环绕 360°(经度)，垂直方向由高度 y 线性映射到 v(覆盖 ±90°)
vec2 SampleCylindricalMap(vec3 direction)
{
	float theta = atan(direction.z, direction.x); // -PI..PI
	float u = theta * INV_2PI + 0.5f;
	float v = clamp(direction.y, -1.0f, 1.0f) * 0.5f + 0.5f;
	return vec2(u, v);
}

void main()
{
	vec3 dir = GetCubeFaceDirection(u_FaceId, WorldPosition.xy);
	vec2 cylindrical_uv = SampleCylindricalMap(dir);
	vec3 color = texture(u_EquirectangularMap, cylindrical_uv).rgb;
	OutColor = vec4(color, 1.0f);
}
