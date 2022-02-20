#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjectionMat;

out vec3 v2f_Position;

void main()
{
	gl_Position = u_ViewProjectionMat * vec4(a_Position, 1.0f);
	v2f_Position = a_Position;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 gl_FragColor;

in vec3 v2f_Position;

uniform vec3 color;
uniform sampler2D u_Texture;

void main()
{
	vec2 uv = v2f_Position.xy;
	vec4 texture_color = texture(u_Texture, uv);
	gl_FragColor = vec4(texture_color.rgb, 1.0f);
	//gl_FragColor = vec4(color, 1.0f);
	//gl_FragColor = vec4(v2f_Position + 0.5f, 1.0f);
}