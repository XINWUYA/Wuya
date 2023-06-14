#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
constexpr float PI = 3.14159265359f;
constexpr float INV_PI = 0.31830988618f;

/* 根据指定变换恢复出平移、旋转和缩放 */
bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

}