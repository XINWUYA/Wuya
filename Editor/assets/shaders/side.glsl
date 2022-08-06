#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;

struct SVextex2Frag
{
	vec2 TexCoord;
};

layout (location = 0) out SVextex2Frag Output;

void main()
{
	gl_Position = a_Position; //vec4(a_Position.xy, a_Position.z * 0.5f + 0.5f, 1.0f);
	gl_Position.z = a_Position.z * 0.5f + 0.5f;

	Output.TexCoord = a_Position.xy * 0.5f + 0.5f;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutFragColor;

struct SVextex2Frag
{
	vec2 TexCoord;
};

layout (location = 0) in SVextex2Frag Input;

uniform sampler2D u_Texture;

void main()
{
	vec2 uv = Input.TexCoord;
	vec4 texture_color = texture(u_Texture, uv);
	//OutFragColor = vec4(texture_color.rgb, 1.0f);
	OutFragColor = texture_color * vec4(1,1,0, 1.0f);
}