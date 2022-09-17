#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform CameraData
{
	mat4 u_ViewProjectionMat;
};

uniform mat4 u_Local2WorldMat;

void main()
{
	gl_Position = u_ViewProjectionMat * u_Local2WorldMat * vec4(a_Position, 1.0f);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); /* 紫色 */
}