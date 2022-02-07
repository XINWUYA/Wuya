#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjectionMat;

out vec3 v2f_Position;
out vec2 v2f_TexCoord;

void main()
{
	gl_Position = u_ViewProjectionMat * vec4(a_Position, 1.0f);

	v2f_Position = a_Position;
	v2f_TexCoord = a_TexCoord;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 gl_FragColor;

in vec3 v2f_Position;
in vec2 v2f_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	vec2 uv = v2f_TexCoord.xy;
	vec4 texture_color = texture(u_Texture, uv);
	gl_FragColor = vec4(texture_color.rgb, 1.0f);
}