#include "Pch.h"
#include "ModelInfo.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Wuya
{
	SubModelInfo::~SubModelInfo()
	{
		for (auto& [stride, buffer_data] : VertexBufferDatas)
		{
			delete[] buffer_data;
		}
	}

	/* 加载Obj模型信息 */
	void ModelInfo::LoadFromObj(const std::string& filepath)
	{
		if (filepath.empty())
		{
			EDITOR_LOG_ERROR("Model filepath is empty.");
		}

		m_Path = filepath;
		const std::string basedir = ExtractFileBaseDir(filepath); /* 模型所在路径 */

		/* 加载模型数据 */
		std::string warn, err;
		bool ret = tinyobj::LoadObj(&m_Attributes, &m_Shapes, &m_Materials, &warn, &err, filepath.c_str(), basedir.c_str());

		if (!warn.empty())
			EDITOR_LOG_WARN("Load Obj Warning: {}.", warn);

		if (!err.empty())
			EDITOR_LOG_ERROR("Load Obj Error: {}.", err);

#if WUYA_DEBUG
		/* 打印模型信息 */
		EDITOR_LOG_DEBUG("Loading Obj: {}", filepath);
		EDITOR_LOG_DEBUG("Vertices  : {}", m_Attributes.vertices.size() / 3);
		EDITOR_LOG_DEBUG("Normals   : {}", m_Attributes.normals.size() / 3);
		EDITOR_LOG_DEBUG("Texcoords : {}", m_Attributes.texcoords.size() / 2);
		EDITOR_LOG_DEBUG("Shapes    : {}", m_Shapes.size());
		EDITOR_LOG_DEBUG("Materials : {}", m_Materials.size());
#endif

		/* 切线和副切线的计算按整个模型为基础 */
		const auto total_vertex_count = m_Attributes.vertices.size();
		std::vector<glm::vec3> tangents;
		tangents.resize(total_vertex_count, glm::vec3(0));
		std::vector<glm::vec3> bitangents;
		bitangents.resize(total_vertex_count, glm::vec3(0));

		std::vector<std::vector<int>> all_shape_indices;
		std::vector<std::vector<glm::vec3>> all_shape_vertices;
		std::vector<std::vector<glm::vec3>> all_shape_normals;
		std::vector<std::vector<glm::vec3>> all_shape_colors;
		std::vector<std::vector<glm::vec2>> all_shape_uvs;
		std::vector<std::pair<glm::vec3, glm::vec3>> all_shape_aabbs;

		/* 每个Shape对应一个MeshSegment */
		for (size_t shape_idx = 0; shape_idx < m_Shapes.size(); ++shape_idx)
		{
			const auto& shape_data = m_Shapes[shape_idx];

			/* 记录AABB信息 */
			auto aabb_min = glm::vec3(std::numeric_limits<float>::max());
			auto aabb_max = glm::vec3(-std::numeric_limits<float>::max());

			const auto indices_count = shape_data.mesh.indices.size();
			std::vector<int> indices;
			indices.reserve(indices_count);
			std::vector<glm::vec3> vertices;
			vertices.reserve(indices_count);
			std::vector<glm::vec3> normals;
			normals.reserve(indices_count);
			std::vector<glm::vec3> colors;
			colors.reserve(indices_count);
			std::vector<glm::vec2> uvs;
			uvs.reserve(indices_count);

			const auto face_count = indices_count / 3;
			for (size_t face_idx = 0; face_idx < face_count; ++face_idx)
			{
				/* 三角形面片的3个顶点索引 */
				tinyobj::index_t vertex_indices[3] = {
					shape_data.mesh.indices[face_idx * 3 + 0],
					shape_data.mesh.indices[face_idx * 3 + 1],
					shape_data.mesh.indices[face_idx * 3 + 2]
				};

				/* 收集3个顶点坐标 */
				ASSERT(vertex_indices[0].vertex_index >= 0 && vertex_indices[1].vertex_index >= 0 && vertex_indices[2].vertex_index >= 0);

				for (int i = 0; i < 3; ++i)
				{
					glm::vec3 vertex(m_Attributes.vertices[vertex_indices[i].vertex_index * 3 + 0], m_Attributes.vertices[vertex_indices[i].vertex_index * 3 + 1], m_Attributes.vertices[vertex_indices[i].vertex_index * 3 + 2]);
					aabb_min = min(vertex, aabb_min);
					aabb_max = max(vertex, aabb_max);
					vertices.emplace_back(vertex);
					indices.emplace_back(vertex_indices[i].vertex_index);
				}

				/* 收集3个顶点法线 */
				bool invalid_normal = false;
				if (!m_Attributes.normals.empty())
				{
					if (vertex_indices[0].normal_index < 0 || vertex_indices[1].normal_index < 0 || vertex_indices[2].normal_index < 0)
					{
						invalid_normal = true;
					}
					else
					{
						for (int i = 0; i < 3; ++i)
						{
							normals.emplace_back(m_Attributes.normals[vertex_indices[i].normal_index * 3 + 0], m_Attributes.normals[vertex_indices[i].normal_index * 3 + 1], m_Attributes.normals[vertex_indices[i].normal_index * 3 + 2]);
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
						colors.emplace_back(m_Attributes.colors[vertex_indices[i].vertex_index * 3 + i], m_Attributes.colors[vertex_indices[i].vertex_index * 3 + i], m_Attributes.colors[vertex_indices[i].vertex_index * 3 + i]);
					}
				}

				/* 收集3个顶点uv */
				float uv[3][2];
				memset(uv, 0.0f, 6 * sizeof(float));
				if (!m_Attributes.texcoords.empty())
				{
					if (vertex_indices[0].texcoord_index >= 0 && vertex_indices[1].texcoord_index >= 0 && vertex_indices[2].texcoord_index >= 0)
					{
						for (int i = 0; i < 3; ++i)
							uvs.emplace_back(m_Attributes.texcoords[vertex_indices[i].texcoord_index * 2], 1.0f - m_Attributes.texcoords[vertex_indices[i].texcoord_index * 2 + 1]);
					}
				}

				/* 计算Tangent和BiTangent */
				if (!m_Attributes.texcoords.empty()) /* 确保存在uv数据 */
				{
					// Ref: https://learnopengl-cn.github.io/05%20Advanced%20Lighting/04%20Normal%20Mapping/#_3
					glm::vec3& v0 = vertices[face_idx * 3 + 0];
					glm::vec3& v1 = vertices[face_idx * 3 + 1];
					glm::vec3& v2 = vertices[face_idx * 3 + 2];

					glm::vec2& w0 = uvs[face_idx * 3 + 0];
					glm::vec2& w1 = uvs[face_idx * 3 + 1];
					glm::vec2& w2 = uvs[face_idx * 3 + 2];

					glm::vec3 edge01 = v1 - v0;
					glm::vec3 edge02 = v2 - v0;
					glm::vec2 uv01 = w1 - w0;
					glm::vec2 uv02 = w2 - w0;

					float f = uv01.x * uv02.y - uv01.y * uv02.x;
					if (abs(f) < 1e-6f)
						f = 1e-6f;
					f = 1.0f / f;
					glm::vec3 tangent(f * (uv02.y * edge01.x - uv01.y * edge02.x), f * (uv02.y * edge01.y - uv01.y * edge02.y), f * (uv02.y * edge01.z - uv01.y * edge02.z));
					glm::vec3 bitangent(f * (-uv02.x * edge01.x + uv01.x * edge02.x), f * (-uv02.x * edge01.y + uv01.x * edge02.y), f * (-uv02.x * edge01.z + uv01.x * edge02.z));
					if (abs(tangent.x) + abs(tangent.y) + abs(tangent.z) <= 0.001f || isnan(tangent.x) || isnan(tangent.y) || isnan(tangent.z) || isnan(bitangent.x) || isnan(bitangent.y) || isnan(bitangent.z))
					{
						tangent = glm::vec3(1.0f, 0.0f, 0.0f);
						bitangent = glm::vec3(0.0f, 0.0f, 1.0f);
					}
					for (int i = 0; i < 3; ++i)
					{
						tangents[vertex_indices[i].vertex_index] += tangent;
						bitangents[vertex_indices[i].vertex_index] += bitangent;
					}
				}
			}

			all_shape_indices.emplace_back(indices);
			all_shape_vertices.emplace_back(vertices);
			all_shape_normals.emplace_back(normals);
			all_shape_colors.emplace_back(colors);
			all_shape_uvs.emplace_back(uvs);
			all_shape_aabbs.emplace_back(aabb_min, aabb_max);
		}

		/* Normalize Tangent and BiTangent */
		for (size_t vertex_idx = 0; vertex_idx < total_vertex_count; ++vertex_idx)
		{
			tangents[vertex_idx] = glm::normalize(tangents[vertex_idx]);
			bitangents[vertex_idx] = glm::normalize(bitangents[vertex_idx]);
		}

		for (size_t shape_idx = 0; shape_idx < m_Shapes.size(); ++shape_idx)
		{
			const auto& vertices = all_shape_vertices[shape_idx];
			auto& normals = all_shape_normals[shape_idx];
			auto& colors = all_shape_colors[shape_idx];
			auto& uvs = all_shape_uvs[shape_idx];
			auto& indices = all_shape_indices[shape_idx];

			const auto& shape_data = m_Shapes[shape_idx];

			SharedPtr<SubModelInfo> sub_model_info = CreateSharedPtr<SubModelInfo>();
			sub_model_info->Name = shape_data.name;

			/* Combine Vertex Data */
			//for (size_t vertex_idx = 0; vertex_idx < vertices.size(); ++vertex_idx)
			//{
			//	const glm::vec3& vertex = vertices[vertex_idx];
			//	const glm::vec3& normal = normals[vertex_idx];
			//	const glm::vec3& color = colors[vertex_idx];
			//	const glm::vec2& uv = uvs[vertex_idx];

			//	int indice_idx = indices[vertex_idx];
			//	const glm::vec3& tangent = tangents[indice_idx];
			//	const glm::vec3& bitangent = bitangents[indice_idx];

			//	glm::vec3 tangent_correct = glm::normalize(tangent - normal * glm::dot(normal, tangent));
			//	// glm::vec3 t = glm::cross(normal, tangent);
			//	// if (glm::dot(t, bitangent) < 0)
			//	// 	tangent_correct *= -1.0f;
			//	sub_model_info->VertexData.emplace_back(vertex.x);
			//	sub_model_info->VertexData.emplace_back(vertex.y);
			//	sub_model_info->VertexData.emplace_back(vertex.z);
			//	sub_model_info->VertexData.emplace_back(normal.x);
			//	sub_model_info->VertexData.emplace_back(normal.y);
			//	sub_model_info->VertexData.emplace_back(normal.z);
			//	sub_model_info->VertexData.emplace_back(color.x);
			//	sub_model_info->VertexData.emplace_back(color.y);
			//	sub_model_info->VertexData.emplace_back(color.z);
			//	sub_model_info->VertexData.emplace_back(uv.x);
			//	sub_model_info->VertexData.emplace_back(uv.y);
			//	sub_model_info->VertexData.emplace_back(tangent_correct.x);
			//	sub_model_info->VertexData.emplace_back(tangent_correct.y);
			//	sub_model_info->VertexData.emplace_back(tangent_correct.z);
			//	sub_model_info->VertexData.emplace_back(bitangent.x);
			//	sub_model_info->VertexData.emplace_back(bitangent.y);
			//	sub_model_info->VertexData.emplace_back(bitangent.z);
			//}

			/* Vertex Buffer */
			/*auto vertex_buffer = VertexBuffer::Create(sub_model_info->VertexData.data(), sub_model_info->VertexData.size() * sizeof(float));
			VertexBufferLayout vertex_buffer_layout = {
				{ "a_Position", BufferDataType::Float3 },
				{ "a_Normal", BufferDataType::Float3 },
				{ "a_Color", BufferDataType::Float3 },
				{ "a_TexCoord", BufferDataType::Float2 },
				{ "a_Tangent", BufferDataType::Float3 },
				{ "a_BiTangent", BufferDataType::Float3 },
			};
			vertex_buffer->SetLayout(vertex_buffer_layout);*/

			/* Vertex Array */
			sub_model_info->VertexArray = VertexArray::Create();
			sub_model_info->VertexArray->Bind();
			{
				auto vertex_buffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(glm::vec3));
				vertex_buffer->SetLayout({ { "a_Position", BufferDataType::Float3 } });
				sub_model_info->VertexArray->AddVertexBuffer(vertex_buffer);
			}
			{
				auto vertex_buffer = VertexBuffer::Create(normals.data(), normals.size() * sizeof(glm::vec3));
				vertex_buffer->SetLayout({ { "a_Normal", BufferDataType::Float3 } });
				sub_model_info->VertexArray->AddVertexBuffer(vertex_buffer);
			}
			{
				auto vertex_buffer = VertexBuffer::Create(colors.data(), colors.size() * sizeof(glm::vec3));
				vertex_buffer->SetLayout({ { "a_Color", BufferDataType::Float3 } });
				sub_model_info->VertexArray->AddVertexBuffer(vertex_buffer);
			}
			{
				auto vertex_buffer = VertexBuffer::Create(uvs.data(), uvs.size() * sizeof(glm::vec2));
				vertex_buffer->SetLayout({ { "a_TexCoord", BufferDataType::Float2 } });
				sub_model_info->VertexArray->AddVertexBuffer(vertex_buffer);
			}

			/* MaterialParams */
			std::filesystem::path relative_dir = std::filesystem::relative(basedir, g_AssetsPath);
			auto& params = sub_model_info->MaterialParams;
			int shape_material_id = shape_data.mesh.material_ids[0];
			if (shape_material_id >= 0 && shape_material_id < m_Materials.size())
			{
				const auto& material_data = m_Materials[shape_material_id];
				/* Name */
				params.Name = material_data.name;
				/* Ambient */
				params.AmbientTexPath = { material_data.ambient_texname.empty() ? "" : (relative_dir / material_data.ambient_texname).generic_string(), !material_data.ambient_texname.empty() };
				params.AmbientFactor = { glm::vec3(material_data.ambient[0], material_data.ambient[1], material_data.ambient[2]), true };
				/* Diffuse/Albedo */
				params.DiffuseTexPath = { material_data.diffuse_texname.empty() ? "" : (relative_dir / material_data.diffuse_texname).generic_string(), !material_data.diffuse_texname.empty() };
				params.DiffuseFactor = { glm::vec3(material_data.diffuse[0], material_data.diffuse[1], material_data.diffuse[2]), true };
				/* Specular */
				params.SpecularTexPath = { material_data.specular_texname.empty() ? "" : (relative_dir / material_data.specular_texname).generic_string(), !material_data.specular_texname.empty() };
				params.SpecularFactor = { glm::vec3(material_data.specular[0], material_data.specular[1], material_data.specular[2]), true };
				/* NormalTex */
				params.BumpTexPath = { material_data.bump_texname.empty() ? "" : (relative_dir / material_data.bump_texname).generic_string(), !material_data.bump_texname.empty() };
				params.DisplacementTexPath = { material_data.displacement_texname.empty() ? "" : (relative_dir / material_data.displacement_texname).generic_string(), !material_data.displacement_texname.empty() };
				/* Roughness */
				params.RoughnessTexPath = { material_data.roughness_texname.empty() ? "" : (relative_dir / material_data.roughness_texname).generic_string(), !material_data.roughness_texname.empty() };
				params.RoughnessFactor = { material_data.roughness, true };
				/* Metallic */
				params.MetallicTexPath = { material_data.metallic_texname.empty() ? "" : (relative_dir / material_data.metallic_texname).generic_string(), !material_data.metallic_texname.empty() };
				params.MetallicFactor = { material_data.metallic, true };
				/* Emission */
				params.EmissionTexPath = { material_data.emissive_texname.empty() ? "" : (relative_dir / material_data.emissive_texname).generic_string(), !material_data.emissive_texname.empty() };
				params.EmissionFactor = { glm::vec3(material_data.emission[0], material_data.emission[1], material_data.emission[2]), true };
				/* ClearCoat */
				params.ClearCoatRoughness = { material_data.clearcoat_roughness, true };
				params.ClearCoatThickness = { material_data.clearcoat_thickness, true };
				/* Others */
				params.TransmittanceColor = { glm::vec3(material_data.transmittance[0], material_data.transmittance[1], material_data.transmittance[2]), true };
				params.IOR = { material_data.ior, true };
			}

			sub_model_info->AABB = all_shape_aabbs[shape_idx];

			m_SubModelInfos.emplace_back(sub_model_info);

			/* 更新模型整体的AABB */
			m_AABB.first = min(m_AABB.first, sub_model_info->AABB.first);
			m_AABB.second = max(m_AABB.second, sub_model_info->AABB.second);
		}
	}

	void ModelInfo::LoadFromPath(const std::string& filepath)
	{
		if (filepath.empty())
		{
			EDITOR_LOG_ERROR("Model filepath is empty.");
		}

		m_Path = filepath;
		
		Assimp::Importer importer;
		const auto* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			EDITOR_LOG_ERROR("Error: Assimp load file({}) failed.", importer.GetErrorString());
			return;
		}

		m_Directory = ExtractFileBaseDir(filepath);
		LoadNode(scene->mRootNode, scene);
	}

	/* 重置数据 */
	void ModelInfo::Reset()
	{
		m_Shapes.clear();
		m_Materials.clear();
		m_SubModelInfos.clear();
		m_Path.clear();
	}

	void ModelInfo::LoadNode(const aiNode* node, const aiScene* scene)
	{
		/* Load当前节点的Mesh */
		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
			LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);

		/* 递归Load所有子节点 */
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
			LoadNode(node->mChildren[i], scene);
	}

	void ModelInfo::LoadMesh(const aiMesh* mesh, const aiScene* scene)
	{
		auto sub_model_info = CreateSharedPtr<SubModelInfo>();
		sub_model_info->Name = mesh->mName.C_Str();
		sub_model_info->VertexCount = mesh->mNumVertices;
		
		sub_model_info->VertexArray = VertexArray::Create();
		sub_model_info->VertexArray->Bind();

		/* Position */
		{
			auto vertex_buffer = VertexBuffer::Create(mesh->mVertices, mesh->mNumVertices * sizeof(glm::vec3));
			vertex_buffer->SetLayout({ { "a_Position", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(vertex_buffer);

			/* Copy Vertices */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, mesh->mVertices, mesh->mNumVertices * sizeof(glm::vec3));
		}

		/* Normal */
		if (mesh->HasNormals())
		{
			auto normal_buffer = VertexBuffer::Create(mesh->mNormals, mesh->mNumVertices * sizeof(glm::vec3));
			normal_buffer->SetLayout({ { "a_Normal", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(normal_buffer);

			/* Copy Normal */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, mesh->mNormals, mesh->mNumVertices * sizeof(glm::vec3));
		}
		else
		{
			std::vector normal(mesh->mNumVertices, glm::vec3(0.0f, 0.0f, 1.0f));
			auto normal_buffer = VertexBuffer::Create(normal.data(), mesh->mNumVertices * sizeof(glm::vec3));
			normal_buffer->SetLayout({ { "a_Normal", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(normal_buffer);

			/* Copy Normal */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, normal.data(), mesh->mNumVertices * sizeof(glm::vec3));
		}

		/* Color */
		if (mesh->HasVertexColors(0))
		{
			auto color_buffer = VertexBuffer::Create(mesh->mColors[0], mesh->mNumVertices * sizeof(glm::vec4));
			color_buffer->SetLayout({ { "a_Color", BufferDataType::Float4 } });
			sub_model_info->VertexArray->AddVertexBuffer(color_buffer);

			/* Copy Color */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 4;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, mesh->mColors[0], mesh->mNumVertices * sizeof(glm::vec4));
		}
		else
		{
			std::vector colors(mesh->mNumVertices, glm::vec4(1.0f));
			auto color_buffer = VertexBuffer::Create(colors.data(), mesh->mNumVertices * sizeof(glm::vec4));
			color_buffer->SetLayout({ { "a_Color", BufferDataType::Float4 } });
			sub_model_info->VertexArray->AddVertexBuffer(color_buffer);

			/* Copy Color */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 4;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, colors.data(), mesh->mNumVertices * sizeof(glm::vec4));
		}

		/* UV */
		if (mesh->HasTextureCoords(0))
		{
			auto uv_buffer = VertexBuffer::Create(mesh->mTextureCoords[0], mesh->mNumVertices * sizeof(glm::vec3));
			uv_buffer->SetLayout({ { "a_TexCoord", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(uv_buffer);

			/* Copy UV */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, mesh->mTextureCoords[0], mesh->mNumVertices * sizeof(glm::vec3));
		}
		else
		{
			std::vector uvs(mesh->mNumVertices, glm::vec3(0.0f));
			auto uv_buffer = VertexBuffer::Create(uvs.data(), mesh->mNumVertices * sizeof(glm::vec3));
			uv_buffer->SetLayout({ { "a_TexCoord", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(uv_buffer);

			/* Copy UV */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, uvs.data(), mesh->mNumVertices * sizeof(glm::vec3));
		}

		/* Tangent */
		if (mesh->HasTangentsAndBitangents())
		{
			auto tangent_buffer = VertexBuffer::Create(mesh->mTangents, mesh->mNumVertices * sizeof(glm::vec3));
			tangent_buffer->SetLayout({ { "a_Tangent", BufferDataType::Float3 } });
			sub_model_info->VertexArray->AddVertexBuffer(tangent_buffer);

			/* Not needed */
			// auto bitangent_buffer = VertexBuffer::Create(mesh->mBitangents, mesh->mNumVertices * sizeof(glm::vec3));
			// bitangent_buffer->SetLayout({ { "a_BiTangent", BufferDataType::Float3 } });
			// sub_model_info->VertexArray->AddVertexBuffer(bitangent_buffer);

			/* Copy Tangent */
			auto& [stride, data] = sub_model_info->VertexBufferDatas.emplace_back();
			stride = 3;
			data = new float[mesh->mNumVertices * stride];
			memcpy(data, mesh->mTangents, mesh->mNumVertices * sizeof(glm::vec3));
		}

		/* 收集indices */
		for (uint32_t face_idx = 0; face_idx < mesh->mNumFaces; ++face_idx)
		{
			auto& face = mesh->mFaces[face_idx];
			for (uint32_t idx = 0; idx < face.mNumIndices; ++idx)
			{
				sub_model_info->Indices.emplace_back(face.mIndices[idx]);
			}
		}
		/* Index Buffer */
		auto index_buffer = IndexBuffer::Create(sub_model_info->Indices.data(), sub_model_info->Indices.size());
		sub_model_info->VertexArray->SetIndexBuffer(index_buffer);

		sub_model_info->VertexArray->Unbind();

		/* MaterialParams */
		std::filesystem::path relative_dir = std::filesystem::relative(m_Directory, g_AssetsPath);

		auto GetTexture = [&](const aiMaterial* mtl, aiTextureType type, std::pair<std::string, bool>& params)
		{
			for (uint32_t i = 0; i < mtl->GetTextureCount(type); ++i)
			{
				aiString str;
				if (mtl->GetTexture(type, i, &str) == AI_SUCCESS)
					params = { (relative_dir / str.C_Str()).generic_string(), true };
			}
		};

		auto GetVec3 = [](const aiMaterial* mtl, const char* pKey, unsigned int type, unsigned int idx, std::pair<glm::vec3, bool>& params)
		{
			aiColor3D color;
			if (mtl->Get(pKey, type, idx, color) == AI_SUCCESS)
				params = { glm::vec3(color.r, color.g, color.b), true };
		};

		auto GetFloat = [](const aiMaterial* mtl, const char* pKey, unsigned int type, unsigned int idx, std::pair<float, bool>& params)
		{
			ai_real value;
			if (mtl->Get(pKey, type, idx, value) == AI_SUCCESS)
				params = { value, true };
		};

		auto material = scene->mMaterials[mesh->mMaterialIndex];
		auto& params = sub_model_info->MaterialParams;

		GetTexture(material, aiTextureType_AMBIENT, params.AmbientTexPath);
		GetTexture(material, aiTextureType_AMBIENT_OCCLUSION, params.AmbientTexPath);
		GetTexture(material, aiTextureType_DIFFUSE, params.DiffuseTexPath);
		GetTexture(material, aiTextureType_SPECULAR, params.SpecularTexPath);
		GetTexture(material, aiTextureType_NORMALS, params.NormalTexPath);
		GetTexture(material, aiTextureType_HEIGHT, params.BumpTexPath);
		GetTexture(material, aiTextureType_DISPLACEMENT, params.DisplacementTexPath);
		GetTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, params.RoughnessTexPath);
		GetTexture(material, aiTextureType_METALNESS, params.MetallicTexPath);
		GetTexture(material, aiTextureType_EMISSIVE, params.EmissionTexPath);
		GetTexture(material, aiTextureType_EMISSION_COLOR, params.EmissionTexPath);
		GetVec3(material, AI_MATKEY_COLOR_AMBIENT, params.AmbientFactor);
		GetVec3(material, AI_MATKEY_COLOR_DIFFUSE, params.DiffuseFactor);
		GetVec3(material, AI_MATKEY_COLOR_SPECULAR, params.SpecularFactor);
		GetFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, params.RoughnessFactor);
		GetFloat(material, AI_MATKEY_METALLIC_FACTOR, params.MetallicFactor);
		GetVec3(material, AI_MATKEY_EMISSIVE_INTENSITY, params.EmissionFactor);
		GetFloat(material, AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, params.ClearCoatRoughness);
		GetFloat(material, AI_MATKEY_CLEARCOAT_FACTOR, params.ClearCoatThickness);
		GetVec3(material, AI_MATKEY_TRANSMISSION_FACTOR, params.TransmittanceColor);
		GetFloat(material, AI_MATKEY_REFRACTI, params.IOR);

		m_SubModelInfos.emplace_back(sub_model_info);

		/* 更新模型整体的AABB */
		for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
		{
			auto vertex = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			sub_model_info->AABB.first = min(sub_model_info->AABB.first, vertex);
			sub_model_info->AABB.second = max(sub_model_info->AABB.second, vertex);
		}

		m_AABB.first = min(m_AABB.first, sub_model_info->AABB.first);
		m_AABB.second = max(m_AABB.second, sub_model_info->AABB.second);
	}
}
