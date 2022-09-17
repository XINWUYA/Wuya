#pragma once

namespace Wuya
{
	/* 导入Obj模型，在模型所在路径中生成对应的.mesh和.mtl文件
	 * 其中mesh文件为二进制文件，包含：
	 * MeshSegmentCount : size_t
	 * each MeshSegment name size : size_t
	 * each MeshSegment name : each MeshSegment name size
	 * each MeshSegment VertexDataSize : size_t
	 * each MeshSegment VertexData : sizeof(float) * each MeshSegment VertexDataSize
	 * each MeshSegment Mtl Id : int
	 * each MeshSegment AABB min : glm::vec3
	 * each MeshSegment AABB max : glm::vec3
	 */
	void ImportObj(const std::string& filepath);
}