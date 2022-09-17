#include "Pch.h"
#include "Mesh.h"
#include "Material.h"

namespace Wuya
{
	MeshSegment::MeshSegment(std::string name)
		: m_DebugName(std::move(name))
	{
	}

	MeshSegment::MeshSegment(std::string name, const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material)
		: m_DebugName(std::move(name)), m_pVertexArray(vertex_array), m_pMaterial(material)
	{
	}
}
