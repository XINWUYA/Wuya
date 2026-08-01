#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

/* 光照视图投影矩阵数组的最大长度（与 C++ 侧 LightUniformData 保持一致） */
#define MAX_LIGHT_VIEW_PROJ 4



layout(std140, binding = 0) uniform ViewUniformBuffer
{
	mat4 u_ViewMat;
	mat4 u_ProjectionMat;
	mat4 u_ViewProjectionMat;
	vec3 u_ViewPos;
	uint u_FrameCounter;
};

layout(std140, binding = 1) uniform ObjectUniformBuffer
{
	mat4 u_Local2WorldMat;
	int  u_ObjectId;
};

layout(std140, binding = 3) uniform LightUniformBuffer
{
	/* 级联阴影的光照视图投影矩阵数组（最多 MAX_LIGHT_VIEW_PROJ 个） */
	mat4 u_LightViewProjectionMat[MAX_LIGHT_VIEW_PROJ];
	vec4 u_ColorIntensity; /* rgb： Color; a: Intensity */
	vec3 u_LightDir;
	uint u_LightType;
	vec3 u_LightPos;
	uint u_CascadeCount;   /* 实际使用的级联数量 */
	vec4 u_CascadeSplits;
	float u_ShadowBias;    /* 阴影深度偏移（来自 ShadowMapInfo::ConstantBias），缓解阴影失真 */
};

#endif // UNIFORMS_GLSL