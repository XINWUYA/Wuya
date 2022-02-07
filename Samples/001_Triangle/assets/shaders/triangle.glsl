#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

out vec3 v2f_Position;

void main()
{
	gl_Position = vec4(a_Position, 1.0f);
	v2f_Position = a_Position;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 gl_FragColor;

in vec3 v2f_Position;

void main()
{
	gl_FragColor = vec4(v2f_Position + 0.5f, 1.0f);
}