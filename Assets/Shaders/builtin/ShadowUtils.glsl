#ifndef SHADOW_UTILS_GLSL
#define SHADOW_UTILS_GLSL

#include "Uniforms.glsl"

/*
 * 级联阴影计算（前向 / 延迟渲染通用）
 *
 * 依赖 LightUniformBuffer 中的 u_CascadeCount、u_LightViewProjectionMat、u_CascadeSplits、
 * u_ShadowBias（binding 固定，天然通用）。由于阴影贴图在不同管线中的 binding 不一致，因此将
 * sampler 作为参数传入，使本函数与具体绑定点解耦。阴影偏移已并入 Light UBO 的 u_ShadowBias。
 *
 *   shadowMap      ：级联阴影贴图数组
 *   world_pos      ：世界空间坐标
 *   view_pos       ：视图空间坐标（用于按深度选择级联）
 *
 * 返回：阴影因子，0 = 完全受光，1 = 完全被遮挡
 */
float CalculateShadow(sampler2DArray shadowMap, vec3 world_pos, vec3 view_pos)
{
	int cascade_id = 0;
	float view_depth = abs(view_pos.z);

	for (int i = 0; i < int(u_CascadeCount) - 1; ++i)
	{
		if (view_depth > u_CascadeSplits[i])
			cascade_id = i + 1;
	}

	mat4 light_vp_mat = u_LightViewProjectionMat[cascade_id];

	vec4 light_space_pos = light_vp_mat * vec4(world_pos, 1.0f);
	light_space_pos /= light_space_pos.w;

	vec3 shadow_coord = light_space_pos.xyz * 0.5f + 0.5f;

	if (shadow_coord.z > 1.0f || shadow_coord.z < 0.0f)
		return 0.0f;

	float current_depth = shadow_coord.z - u_ShadowBias;

	/* 3x3 PCF 软阴影 */
	float shadow = 0.0f;
	vec2 texel_size = 1.0f / vec2(textureSize(shadowMap, 0).xy);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			vec2 offset = vec2(x, y) * texel_size;
			float closest_depth = texture(shadowMap, vec3(shadow_coord.xy + offset, float(cascade_id))).r;
			shadow += current_depth > closest_depth ? 1.0f : 0.0f;
		}
	}

	return shadow / 9.0f;
}

#endif // SHADOW_UTILS_GLSL
