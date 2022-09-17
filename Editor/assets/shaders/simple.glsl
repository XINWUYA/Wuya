#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjectionMat;
};

uniform mat4 u_Local2WorldMat;

struct SVextex2Frag
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

layout (location = 0) out SVextex2Frag Output;

void main()
{
	gl_Position = u_ViewProjectionMat * u_Local2WorldMat * vec4(a_Position, 1.0f);

	Output.Position = a_Position;
	Output.Normal = a_Normal;
	Output.TexCoord = a_TexCoord;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutGBufferAlbedo;
layout(location = 1) out vec4 OutGBufferNormal;
layout(location = 2) out vec4 OutGBufferRoughness;

struct SVextex2Frag
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
};

layout (location = 0) in SVextex2Frag Input;

uniform sampler2D u_AlbedoTexture;
uniform sampler2D u_ColorTexture;

void main()
{
	vec4 albedo = texture(u_AlbedoTexture, Input.TexCoord);
	vec4 color = texture(u_ColorTexture, Input.TexCoord);
	
	OutGBufferAlbedo = vec4(albedo.rgb, 1.0f);
	OutGBufferNormal = vec4(Input.Normal, 1.0f);
	OutGBufferRoughness = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}