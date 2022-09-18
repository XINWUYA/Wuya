#pragma once

namespace Wuya
{
	/* ����Objģ�ͣ���ģ������·�������ɶ�Ӧ��.mesh��.mtl�ļ�
	 * ����mesh�ļ�Ϊ�������ļ���������
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