#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_TextureIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityId;

layout(std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjectionMat;
};

struct SVextex2Frag
{
	vec3 Position;
	vec2 TexCoord;
	vec4 Color;
	float TextureIndex;
	float TilingFactor;
};

layout (location = 0) out SVextex2Frag Output;
layout (location = 5) out flat int EntityId;

void main()
{
	gl_Position = u_ViewProjectionMat * vec4(a_Position, 1.0f);

	Output.Position = a_Position;
	Output.TexCoord = a_TexCoord;
	Output.Color = a_Color;
	Output.TextureIndex = a_TextureIndex;
	Output.TilingFactor = a_TilingFactor;
	EntityId = a_EntityId;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 gl_FragColor;
//layout(location = 1) out int gl_FragColor2;

struct SVextex2Frag
{
	vec3 Position;
	vec2 TexCoord;
	vec4 Color;
	float TextureIndex;
	float TilingFactor;
};

layout (location = 0) in SVextex2Frag Input;
layout (location = 5) in flat int EntityId;

layout (binding = 0) uniform sampler2D u_Texture[32];

void main()
{
	vec2 uv = Input.TexCoord * Input.TilingFactor;
	vec4 texture_color = texture(u_Texture[int(Input.TextureIndex)], uv);
	gl_FragColor = vec4(texture_color.rgb, 1.0f) * Input.Color;
	//gl_FragColor2 = EntityId;
}