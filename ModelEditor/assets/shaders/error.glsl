#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

#include "builtin/uniforms.glsl"

void main()
{
	gl_Position = u_ViewProjectionMat * u_Local2WorldMat * vec4(a_Position, 1.0f);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 OutColor;
layout(location = 5) out int  OutObjectId;

#include "builtin/uniforms.glsl"

void main()
{
	OutColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); /* 紫色 */

	OutObjectId = u_ObjectId;
}