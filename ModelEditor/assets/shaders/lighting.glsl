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
	
	float dotNL = dot(gbuffer.WorldNormal, -u_LightDir);
	vec3 diffuse = dotNL * gbuffer.Albedo * u_ColorIntensity.rgb * u_ColorIntensity.a;

	OutFragColor = vec4(diffuse, 1.0f);
}