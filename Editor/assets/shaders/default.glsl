#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Color;
layout(location = 3) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjectionMat;
};

uniform mat4 u_Local2WorldMat;

struct SVextex2Frag
{
	vec3 Position;
	vec3 Normal;
	vec3 BaseColor;
	vec2 TexCoord;
};

layout (location = 0) out SVextex2Frag Output;

void main()
{
	gl_Position = u_ViewProjectionMat * u_Local2WorldMat * vec4(a_Position, 1.0f);

	Output.Position = a_Position;
	Output.Normal = a_Normal;
	Output.BaseColor = a_Color;
	Output.TexCoord = a_TexCoord;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutGBufferAlbedo;
layout(location = 1) out vec4 OutGBufferSpecular;
layout(location = 2) out vec4 OutGBufferNormal;
layout(location = 3) out vec4 OutGBufferRoughnessMetallic;
layout(location = 4) out vec4 OutGBufferEmissive;

struct SVextex2Frag
{
	vec3 Position;
	vec3 Normal;
	vec3 BaseColor;
	vec2 TexCoord;
};

layout (location = 0) in SVextex2Frag Input;

layout(binding = 0) uniform sampler2D u_AlbedoTexture;
layout(binding = 1) uniform sampler2D u_SpecularTexture;
layout(binding = 2) uniform sampler2D u_NormalTexture;
layout(binding = 3) uniform sampler2D u_RoughnessTexture;
layout(binding = 4) uniform sampler2D u_MetallicTexture;
layout(binding = 5) uniform sampler2D u_EmissiveTexture;


void main()
{
	vec3 albedo = texture(u_AlbedoTexture, Input.TexCoord).rgb;
	vec3 specular = texture(u_SpecularTexture, Input.TexCoord).rgb;
	vec3 normal = texture(u_NormalTexture, Input.TexCoord).rgb;
	float roughness = texture(u_RoughnessTexture, Input.TexCoord).r;
	float metallic = texture(u_MetallicTexture, Input.TexCoord).r;
	vec3 emissive = texture(u_EmissiveTexture, Input.TexCoord).rgb;

	OutGBufferAlbedo = vec4(albedo, 1.0f);
	OutGBufferSpecular = vec4(specular, 1.0f);
	OutGBufferNormal = vec4(normal, 1.0f);
	OutGBufferRoughnessMetallic = vec4(roughness, metallic, 0.0f, 1.0f);
	OutGBufferEmissive = vec4(emissive, 1.0f);
}