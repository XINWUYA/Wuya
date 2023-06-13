#type vertex
#version 450 core

struct SVextex2Frag
{
	vec2 TexCoord;
};

layout(location = 0) in vec4 a_Position;
layout(location = 0) out SVextex2Frag Output;

void main()
{
	gl_Position = a_Position; //vec4(a_Position.xy, a_Position.z * 0.5f + 0.5f, 1.0f);
	gl_Position.z = a_Position.z * 0.5f + 0.5f;

	Output.TexCoord = a_Position.xy * 0.5f + 0.5f;
}


#type fragment
#version 450 core

#include "builtin/Uniforms.glsl"
#include "builtin/GBuffer.glsl"
#include "builtin/Math.glsl"
#include "builtin/BRDF.glsl"

struct SVextex2Frag
{
	vec2 TexCoord;
};

layout(location = 0) out vec4 OutFragColor;
layout(location = 0) in SVextex2Frag Input;

void main()
{
	SGBufferData gbuffer;
	CalculateGBuffer(gbuffer, Input.TexCoord);
	
	vec3 l = -normalize(u_LightDir);
	vec3 v = normalize(u_ViewPos - gbuffer.WorldPosition);

	// Direct Lighting
	vec3 lighting_result = max(vec3(0.0f), BRDF(l, v, gbuffer.WorldNormal, gbuffer.Metallic, gbuffer.Roughness, gbuffer.Albedo));
	// Ambient
	lighting_result += max(vec3(0.0f), gbuffer.Ambient * gbuffer.Albedo * gbuffer.AO);
	// Emissive
	lighting_result += gbuffer.Emission;

	//OutFragColor = vec4(gbuffer.WorldNormal, 1.0f);
	OutFragColor = vec4(lighting_result, 1.0f);
}