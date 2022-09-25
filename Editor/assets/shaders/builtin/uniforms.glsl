#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout(std140, binding = 0) uniform ViewUniformBuffer
{
	mat4 u_ViewProjectionMat;
	uint u_FrameCounter;
};

layout(std140, binding = 1) uniform ObjectUniformBuffer
{
	mat4 u_Local2WorldMat;
	int  u_ObjectId;
};

#endif // UNIFORMS_GLSL