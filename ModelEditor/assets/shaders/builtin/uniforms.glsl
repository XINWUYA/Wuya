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

layout(std140, binding = 3) uniform LightUniformBuffer
{
	vec4 u_ColorIntensity;
	vec3 u_LightDir;
	vec3 u_LightPos;
	uint u_LightType;
};

#endif // UNIFORMS_GLSL