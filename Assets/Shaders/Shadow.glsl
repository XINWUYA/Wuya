#type vertex
#version 410 core

layout(location = 0) in vec3 a_Position;

/* 当前级联索引：每级联渲染前由 CPU 端设置，对应 u_LightViewProjectionMat 的数组下标 */
layout(location = 0) uniform int u_CascadeIndex;

#include "builtin/Uniforms.glsl"

void main()
{
    gl_Position = u_LightViewProjectionMat[u_CascadeIndex] * u_Local2WorldMat * vec4(a_Position, 1.0f);
}


#type fragment
#version 410 core

void main()
{
    // 深度值会自动写入深度附件
    // 这里不需要输出颜色
}
