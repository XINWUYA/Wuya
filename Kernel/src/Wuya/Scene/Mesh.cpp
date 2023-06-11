#include "Pch.h"
#include "Mesh.h"
#include "Material.h"

namespace Wuya
{
	MeshSegment::MeshSegment(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material)
		: m_DebugName(name), m_MeshPrimitive(mesh_primitive), m_pMaterial(material)
	{
	}

	SharedPtr<MeshSegment> MeshSegment::Create(const std::string& name, const MeshPrimitive& mesh_primitive, const SharedPtr<Material>& material)
	{
		if (!material)
			return nullptr;

		return CreateSharedPtr<MeshSegment>(name, mesh_primitive, material);
	}
}
