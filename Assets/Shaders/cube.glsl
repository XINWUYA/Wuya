#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in int a_EntityId;

#include "builtin/Uniforms.glsl"

struct SVextex2Frag
{
	vec3 Position;
	vec2 TexCoord;
};

layout (location = 0) out SVextex2Frag Output;
layout (location = 2) out flat int EntityId;

void main()
{
	gl_Position = u_ViewProjectionMat * u_Local2WorldMat * vec4(a_Position, 1.0f);

	Output.Position = a_Position;
	Output.TexCoord = a_TexCoord;
	EntityId = a_EntityId;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutGBufferAlbedo;
layout(location = 1) out vec4 OutGBufferNormal;
layout(location = 2) out vec4 OutGBufferRoughness;
layout(location = 4) out int OutEntityId;

struct SVextex2Frag
{
	vec3 Position;
	vec2 TexCoord;
};

layout (location = 0) in SVextex2Frag Input;
layout (location = 2) in flat int EntityId;

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_ColorTexture;

void main()
{
	vec4 albedo = texture(u_AlbedoTexture, Input.TexCoord);
	vec4 color = texture(u_ColorTexture, Input.TexCoord);
	//OutFragColor = vec4(albedo.rgb * color.rgb, 1.0f);
	OutGBufferAlbedo = vec4(albedo.rgb, 1.0f);
	OutGBufferNormal = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	OutGBufferRoughness = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	OutEntityId = EntityId;
}