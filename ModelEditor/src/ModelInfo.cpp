#include "Pch.h"
#include "ModelInfo.h"

namespace Wuya
{
	/* 加载Obj模型信息 */
	void ModelInfo::LoadFromObj(const std::string& filepath)
	{
		if (filepath.empty())
		{
			CORE_LOG_ERROR("Model filepath is empty.");
		}

		m_Path = filepath;
		const std::string basedir = ExtractFileBaseDir(filepath); /* 模型所在路径 */

		/* 加载模型数据 */
		std::string warn, err;
		bool ret = tinyobj::LoadObj(&m_Attributes, &m_Shapes, &m_Materials, &warn, &err, filepath.c_str(), basedir.c_str());

		if (!warn.empty())
			CORE_LOG_WARN("Load Obj Warning: {}.", warn);

		if (!err.empty())
			CORE_LOG_ERROR("Load Obj Error: {}.", err);

#if WUYA_DEBUG
		/* 打印模型信息 */
		CORE_LOG_DEBUG("Loading Obj: {}", filepath);
		CORE_LOG_DEBUG("Vertices  : {}", m_Attributes.vertices.size() / 3);
		CORE_LOG_DEBUG("Normals   : {}", m_Attributes.normals.size() / 3);
		CORE_LOG_DEBUG("Texcoords : {}", m_Attributes.texcoords.size() / 2);
		CORE_LOG_DEBUG("Shapes    : {}", m_Shapes.size());
		CORE_LOG_DEBUG("Materials : {}", m_Materials.size());
#endif

		/* 每个Shape对应一个MeshSegment */
		for (size_t shape_idx = 0; shape_idx < m_Shapes.size(); ++shape_idx)
		{
			/* 填充VertexData */
			std::vector<float> data;

			/* 记录AABB信息 */
			glm::vec3 aabb_min = glm::vec3(std::numeric_limits<float>::max());
			glm::vec3 aabb_max = glm::vec3(-std::numeric_limits<float>::max());

			const auto& shape_data = m_Shapes[shape_idx];
			const uint32_t face_count = shape_data.mesh.indices.size() / 3;
			for (size_t face_idx = 0; face_idx < face_count; ++face_idx)
			{
				/* 三角形面片的3个顶点索引 */
				auto index0 = shape_data.mesh.indices[face_idx * 3 + 0];
				auto index1 = shape_data.mesh.indices[face_idx * 3 + 1];
				auto index2 = shape_data.mesh.indices[face_idx * 3 + 2];

				/* 收集3个顶点坐标 */
				ASSERT(index0.vertex_index >= 0 && index1.vertex_index >= 0 && index2.vertex_index >= 0);
				float vertex[3][3];
				for (int i = 0; i < 3; ++i)
				{
					/* x->y->z值依次获取 */
					vertex[0][i] = m_Attributes.vertices[index0.vertex_index * 3 + i];
					vertex[1][i] = m_Attributes.vertices[index1.vertex_index * 3 + i];
					vertex[2][i] = m_Attributes.vertices[index2.vertex_index * 3 + i];

					/* 记录AABB */
					aabb_min[i] = std::min(vertex[0][i], aabb_min[i]);
					aabb_min[i] = std::min(vertex[1][i], aabb_min[i]);
					aabb_min[i] = std::min(vertex[2][i], aabb_min[i]);
					aabb_max[i] = std::max(vertex[0][i], aabb_max[i]);
					aabb_max[i] = std::max(vertex[1][i], aabb_max[i]);
					aabb_max[i] = std::max(vertex[2][i], aabb_max[i]);
				}

				/* 收集3个顶点法线 */
				float normal[3][3];
				bool invalid_normal = false;
				if (!m_Attributes.normals.empty())
				{
					if (index0.normal_index < 0 || index1.normal_index < 0 || index2.normal_index < 0)
					{
						invalid_normal = true;
					}
					else
					{
						for (int i = 0; i < 3; ++i)
						{
							/* x->y->z值依次获取 */
							normal[0][i] = m_Attributes.normals[index0.normal_index * 3 + i];
							normal[1][i] = m_Attributes.normals[index1.normal_index * 3 + i];
							normal[2][i] = m_Attributes.normals[index2.normal_index * 3 + i];
						}
					}
				}

				if (invalid_normal)
				{
					/* todo: 生成法线 */
				}

				/* 收集3个顶点颜色 */
				float color[3][3];
				memset(color, 1.0f, 9 * sizeof(float)); /* 默认为白色 */
				if (!m_Attributes.colors.empty())
				{
					for (int i = 0; i < 3; ++i)
					{
						/* x->y->z值依次获取 */
						color[0][i] = m_Attributes.colors[index0.vertex_index * 3 + i];
						color[1][i] = m_Attributes.colors[index1.vertex_index * 3 + i];
						color[2][i] = m_Attributes.colors[index2.vertex_index * 3 + i];
					}
				}

				/* 收集3个顶点uv */
				float uv[3][2];
				memset(uv, 0.0f, 6 * sizeof(float));
				if (!m_Attributes.texcoords.empty())
				{
					if (index0.texcoord_index >= 0 && index1.texcoord_index >= 0 && index2.texcoord_index >= 0)
					{
						uv[0][0] = m_Attributes.texcoords[index0.texcoord_index * 2];
						uv[0][1] = 1.0f - m_Attributes.texcoords[index0.texcoord_index * 2 + 1];
						uv[1][0] = m_Attributes.texcoords[index1.texcoord_index * 2];
						uv[1][1] = 1.0f - m_Attributes.texcoords[index1.texcoord_index * 2 + 1];
						uv[2][0] = m_Attributes.texcoords[index2.texcoord_index * 2];
						uv[2][1] = 1.0f - m_Attributes.texcoords[index2.texcoord_index * 2 + 1];
					}
				}

				/* Combine */
				for (int v = 0; v < 3; ++v)
				{
					data.emplace_back(vertex[v][0]);
					data.emplace_back(vertex[v][1]);
					data.emplace_back(vertex[v][2]);
					data.emplace_back(normal[v][0]);
					data.emplace_back(normal[v][1]);
					data.emplace_back(normal[v][2]);
					data.emplace_back(color[v][0]);
					data.emplace_back(color[v][1]);
					data.emplace_back(color[v][2]);
					data.emplace_back(uv[v][0]);
					data.emplace_back(uv[v][1]);
				}
			}

			/* Vertex Buffer */
			auto vertex_buffer = VertexBuffer::Create(data.data(), data.size() * sizeof(float));
			VertexBufferLayout vertex_buffer_layout = {
				{ "a_Position", BufferDataType::Float3 },
				{ "a_Normal", BufferDataType::Float3 },
				{ "a_Color", BufferDataType::Float3 },
				{ "a_TexCoord", BufferDataType::Float2 },
			};
			vertex_buffer->SetLayout(vertex_buffer_layout);

			/* Vertex Array */
			auto vertex_array = VertexArray::Create();
			vertex_array->Bind();
			vertex_array->AddVertexBuffer(vertex_buffer);

			/* MaterialParams */
			MaterialParams params{};
			int shape_material_id = shape_data.mesh.material_ids[0];
			if (shape_material_id >= 0 && shape_material_id < m_Materials.size())
			{
				const auto& material_data = m_Materials[shape_material_id];

				/* Ambient */
				params.AmbientTexPath = material_data.ambient_texname.empty() ? "" : basedir + material_data.ambient_texname;
				params.Ambient = glm::vec3(material_data.ambient[0], material_data.ambient[1], material_data.ambient[2]);
				/* Diffuse/Albedo */
				params.DiffuseTexPath = material_data.diffuse_texname.empty() ? "" : basedir + material_data.diffuse_texname,
				params.Diffuse = glm::vec3(material_data.diffuse[0], material_data.diffuse[1], material_data.diffuse[2]),
				/* Specular */
				params.SpecularTexPath = material_data.specular_texname.empty() ? "" : basedir + material_data.specular_texname;
				params.Specular = glm::vec3(material_data.specular[0], material_data.specular[1], material_data.specular[2]);
				/* NormalTex */
				params.BumpTexPath = material_data.bump_texname.empty() ? "" : basedir + material_data.bump_texname;
				params.DisplacementTexPath = material_data.displacement_texname.empty() ? "" : basedir + material_data.displacement_texname;
				/* Roughness */
				params.RoughnessTexPath = material_data.roughness_texname.empty() ? "" : basedir + material_data.roughness_texname;
				params.Roughness = material_data.roughness;
				/* Metallic */
				params.MetallicTexPath = material_data.metallic_texname.empty() ? "" : basedir + material_data.metallic_texname;
				params.Metallic = material_data.metallic;
				/* Emission */
				params.EmissionTexPath = material_data.emissive_texname.empty() ? "" : basedir + material_data.emissive_texname;
				params.Emission = glm::vec3(material_data.emission[0], material_data.emission[1], material_data.emission[2]);
				/* ClearCoat */
				params.ClearCoatRoughness = material_data.clearcoat_roughness;
				params.ClearCoatThickness = material_data.clearcoat_thickness;
				/* Others */
				params.Transmittance = glm::vec3(material_data.transmittance[0], material_data.transmittance[1], material_data.transmittance[2]);
				params.IOR = material_data.ior;
			}

			m_SubModelInfos.emplace_back(shape_data.name, vertex_array, params, std::make_pair(aabb_min, aabb_max));

			/* 更新模型整体的AABB */
			m_AABB.first.x = std::min(m_AABB.first.x, aabb_min.x);
			m_AABB.first.y = std::min(m_AABB.first.y, aabb_min.y);
			m_AABB.first.z = std::min(m_AABB.first.z, aabb_min.z);
			m_AABB.second.x = std::max(m_AABB.second.x, aabb_max.x);
			m_AABB.second.y = std::max(m_AABB.second.y, aabb_max.y);
			m_AABB.second.z = std::max(m_AABB.second.z, aabb_max.z);

			/* Material */
			/*SharedPtr<Material> material = CreateSharedPtr<Material>();
			material->SetShader(ShaderLibrary::Instance().GetOrLoad("assets/shaders/default.glsl"));*/

			//int shape_material_id = shape_data.mesh.material_ids[0];
			//if (shape_material_id >= 0 && shape_material_id < m_Materials.size())
			//{
			//	const auto& material_data = m_Materials[shape_material_id];
			//	
			//	{
			//		/* Ambient */
			//		material_data.ambient_texname.empty()	? "" : basedir + material_data.ambient_texname,
			//		glm::vec3(material_data.ambient[0], material_data.ambient[1], material_data.ambient[2]),
			//		/* Diffuse/Albedo */
			//		material_data.diffuse_texname.empty()	? "" : basedir + material_data.diffuse_texname,
			//		glm::vec3(material_data.diffuse[0], material_data.diffuse[1], material_data.diffuse[2]),

			//		/* Specular */
			//		material_data.specular_texname.empty()	? "" : basedir + material_data.specular_texname,
			//		glm::vec3(material_data.specular[0], material_data.specular[1], material_data.specular[2]),

			//		/* NormalTex */
			//		material_data.bump_texname.empty()		? "" : basedir + material_data.bump_texname,

			//		/* Roughness */
			//		material_data.roughness_texname.empty() ? "" : basedir + material_data.roughness_texname,	
			//		material_data.roughness,

			//		/* Metallic */
			//		material_data.metallic_texname.empty()	? "" : basedir + material_data.metallic_texname,	
			//		material_data.metallic,

			//		/* Emission */
			//		material_data.emissive_texname.empty()	? "" : basedir + material_data.emissive_texname,
			//		glm::vec3(material_data.emission[0], material_data.emission[1], material_data.emission[2]),

			//		/* ClearCoat */
			//		material_data.clearcoat_roughness,
			//		material_data.clearcoat_thickness,

			//		/* Others */
			//		glm::vec3(material_data.transmittance[0], material_data.transmittance[1], material_data.transmittance[2]),
			//		material_data.ior,
			//	};
			//	m_MaterialParams.emplace_back(param);
			//	
			//	//constexpr TextureLoadConfig texture_load_config = { false };
			//	//if (!param.DiffuseTexPath.empty()) /* AlbedoTex */
			//	//	material->SetTexture(Texture::Create(param.DiffuseTexPath, texture_load_config), TextureSlot::Albedo);
			//	//if (!param.SpecularTexPath.empty()) /* SpecularTex */
			//	//	material->SetTexture(Texture::Create(param.SpecularTexPath, texture_load_config), TextureSlot::Specular);
			//	//if (!param.NormalTexPath.empty()) /* NormalTex */
			//	//	material->SetTexture(Texture::Create(param.NormalTexPath, texture_load_config), TextureSlot::Normal);
			//	//if (!param.RoughnessTexPath.empty()) /* RoughnessTex */
			//	//	material->SetTexture(Texture::Create(param.RoughnessTexPath, texture_load_config), TextureSlot::Roughness);
			//	//if (!param.MetallicTexPath.empty()) /* MetallicTex */
			//	//	material->SetTexture(Texture::Create(param.MetallicTexPath, texture_load_config), TextureSlot::Metallic);
			//	//if (!param.EmissiveTexPath.empty()) /* EmissiveTex */
			//	//	material->SetTexture(Texture::Create(param.EmissiveTexPath, texture_load_config), TextureSlot::Emissive);

			//	//material->SetParameters("Ambient", param.Ambient);
			//	//material->SetParameters("Diffuse", param.Diffuse);
			//	//material->SetParameters("Specular", param.Specular);
			//	//material->SetParameters("Emission", param.Emission);
			//	//material->SetParameters("Metallic", param.Metallic);
			//	//material->SetParameters("Roughness", param.Roughness);
			//}

			///* 创建MeshSegment */
			//auto mesh_segment = CreateSharedPtr<MeshSegment>(shape_data.name, vertex_array, material);
			//mesh_segment->SetAABB(aabb_min, aabb_max);
			//m_MeshSegments.emplace_back(mesh_segment);

		}
	}

	/* 保存模型 */
	void ModelInfo::Save(const std::string& filepath)
	{
		/* todo */
	}

	/* 重置数据 */
	void ModelInfo::Reset()
	{
		m_Shapes.clear();
		m_Materials.clear();
		m_SubModelInfos.clear();
		m_Path.clear();
	}
}
