#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Color;
layout(location = 3) in vec2 a_TexCoord;

#include "builtin/Uniforms.glsl"

// layout (location = 0) uniform mat4 u_ViewProjectionMat;
// layout (location = 1) uniform mat4 u_Local2WorldMat;

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

#include "common.glsl"
#include "builtin/Math.glsl"
#include "builtin/Uniforms.glsl"
#include "builtin/MaterialCommon.glsl"

layout(location = 0) out vec4 GBufferTexture0; /* rgb: Albedo; a: Roughness */
layout(location = 1) out vec4 GBufferTexture1; /* rgb: WorldNormal; a: Metallic */
layout(location = 2) out vec4 GBufferTexture2; /* rgb: Specular; a: AO */
layout(location = 3) out vec4 GBufferTexture3; /* rgb: Emission; a: IOR */
layout(location = 4) out vec4 GBufferTexture4; /* rgb: Ambient; a: Anisotropy */
layout(location = 5) out vec4 GBufferTexture5; /* rgb: WorldPosition */
layout(location = 6) out int  ObjectId;

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

void CalculateMaterial(inout SMaterialInput mtl)
{
	
	mtl.Albedo = texture(u_AlbedoTexture, Input.TexCoord).rgb;
	mtl.Normal = texture(u_NormalTexture, Input.TexCoord).rgb;
	mtl.Roughness = texture(u_RoughnessTexture, Input.TexCoord).r;
	mtl.Metallic = texture(u_MetallicTexture, Input.TexCoord).r;
	mtl.Specular = texture(u_SpecularTexture, Input.TexCoord).rgb;
	mtl.Emission = texture(u_EmissiveTexture, Input.TexCoord).rgb;
	mtl.Ambient = vec3(0.0f, 0.0f, 0.0f);
	mtl.Anisotropy = 0.0f;
	mtl.AO = 1.0f;
	mtl.IOR = 0.0f;
	mtl.ClearCoatThickness = 0.0f;
	mtl.ClearCoatRoughness = 0.0f;
}

void main()
{
	
	SMaterialInput mtl;
	CalculateMaterial(mtl);
	
	GBufferTexture0 = vec4(mtl.Albedo, mtl.Roughness);
	GBufferTexture1 = vec4(mtl.Normal, mtl.Metallic);
	GBufferTexture2 = vec4(mtl.Specular, mtl.AO);
	GBufferTexture3 = vec4(mtl.Emission, mtl.IOR);
	GBufferTexture4 = vec4(mtl.Ambient, mtl.Anisotropy);
	GBufferTexture5 = vec4(Input.Position, 1.0f);
	
	ObjectId = u_ObjectId;
}