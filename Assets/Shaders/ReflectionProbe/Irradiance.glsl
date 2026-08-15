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

layout(binding = 0) uniform samplerCube u_EnvironmentMap;
layout(location = 0) uniform int u_FaceId;

void main()
{
	vec3 normal = GetCubeFaceDirection(u_FaceId, WorldPosition.xy);

	// 切空间：以normal为z轴，构造tangent/bitex协调基
	vec3 up = abs(normal.z) < 0.999f ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);

	vec3 irradiance = vec3(0.0f);

	const float k_SampleDelta = 0.025f;
	float sample_count = 0.0f;
	for (float phi = 0.0f; phi < 2.0f * PI; phi += k_SampleDelta)
	{
		for (float theta = 0.0f; theta < 0.5f * PI; theta += k_SampleDelta)
		{
			// 球坐标 -> 切空间
			vec3 tangent_sample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// 切空间 -> 世界空间
			vec3 sample_vec = tangent_sample.x * tangent + tangent_sample.y * bitangent + tangent_sample.z * normal;

			irradiance += texture(u_EnvironmentMap, sample_vec).rgb * cos(theta) * sin(theta);
			++sample_count;
		}
	}

	irradiance = PI * irradiance * (1.0f / sample_count);
	OutColor = vec4(irradiance, 1.0f);
}
