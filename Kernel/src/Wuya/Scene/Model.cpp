#include "Pch.h"
#include "Model.h"

#include <tinyxml2.h>

#include "Mesh.h"
#include "ModelImporter.h"
#include "Wuya/Common/Math.h"
#include "Wuya/Renderer/Buffer.h"
#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"
#include "Wuya/Renderer/VertexArray.h"

namespace Wuya
{
	/* ����һ��Cube���͵�ģ�� */
	static SharedPtr<Model> CreateCube(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinCube");

		/* ���MeshSegment */
		{
			// Cube vertices
			static constexpr float vertices[] = {
				/* Position-----------Normal---------------TexCoord */
				/* Bottom */
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
				 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
				 0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
				-0.5f,  0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
				/* Top */			  			    
				-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
				/* Left */						    
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
				-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
				-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
				-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
				/* Right */
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
				 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
				 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
				 /* Back */
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
				 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
				 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
				-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
				/* Front */
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
				 0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
				 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
				-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
				-0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
			};

			// Vertex Array
			SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
			VertexBufferLayout vertex_buffer_layout = {
				{ "a_Position", BufferDataType::Float3 },
				{ "a_Normal", BufferDataType::Float3 },
				{ "a_TexCoord", BufferDataType::Float2 },
			};
			vertex_buffer->SetLayout(vertex_buffer_layout);

			auto vertex_array = VertexArray::Create();
			vertex_array->Bind();
			vertex_array->AddVertexBuffer(vertex_buffer);

			model->AddMeshSegment(CreateSharedPtr<MeshSegment>("BuiltinCube", vertex_array, material));
		}

		return model;
	}

	/* ����һ��Sphere���͵����� */
	static SharedPtr<Model> CreateSphere(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinSphere");

		constexpr int32_t segments_x = 32;
		constexpr int32_t segments_y = 32;

		/* Calculate vertices, normals, uvs */
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		constexpr int32_t vertex_count = (segments_x + 1) * (segments_y + 1);
		vertices.reserve(vertex_count);
		normals.reserve(vertex_count);
		uvs.reserve(vertex_count);

		for (int32_t x = 0; x <= segments_x; ++x)
		{
			float s_x = static_cast<float>(x) / segments_x;
			for (int32_t y = 0; y <= segments_y; ++y)
			{
				float s_y = static_cast<float>(y) / segments_y;

				glm::vec3 position(
					std::cos(s_x * 2.0f * PI) * std::sin(s_y * PI),
					std::cos(s_y * PI),
					std::sin(s_x * 2.0f * PI) * std::sin(s_y * PI)
				);

				vertices.emplace_back(position);
				normals.emplace_back(position); /* ������������ԭ�������normal��position��� */
				uvs.emplace_back(s_x, s_y);
			}
		}

		/* Calculate indices */
		std::vector<uint32_t> indices;
		bool odd_row = false;
		for (int32_t y = 0; y < segments_y; ++y)
		{
			if (!odd_row) // even rows: y == 0, y == 2; and so on
			{
				for (int32_t x = 0; x <= segments_x; ++x)
				{
					indices.emplace_back(y * (segments_x + 1) + x);
					indices.emplace_back((y + 1) * (segments_x + 1) + x);
				}
			}
			else
			{
				for (int32_t x = segments_x; x >= 0; --x)
				{
					indices.emplace_back((y + 1) * (segments_x + 1) + x);
					indices.emplace_back(y * (segments_x + 1) + x);
				}
			}
			odd_row = !odd_row;
		}

		auto vertex_array = VertexArray::Create();
		vertex_array->Bind();
		/* vertices */
		{
			auto vertex_buffer = VertexBuffer::Create(vertices.data(), vertex_count * sizeof(glm::vec3));
			vertex_buffer->SetLayout({ { "a_Position", BufferDataType::Float3 } });
			vertex_array->AddVertexBuffer(vertex_buffer);
		}
		/* normals */
		{
			auto vertex_buffer = VertexBuffer::Create(normals.data(), vertex_count * sizeof(glm::vec3));
			vertex_buffer->SetLayout({ { "a_Normal", BufferDataType::Float3 } });
			vertex_array->AddVertexBuffer(vertex_buffer);
		}
		/* uvs */
		{
			auto vertex_buffer = VertexBuffer::Create(uvs.data(), vertex_count * sizeof(glm::vec2));
			vertex_buffer->SetLayout({ { "a_TexCoord", BufferDataType::Float2 } });
			vertex_array->AddVertexBuffer(vertex_buffer);
		}
		/* indices */
		{
			auto index_buffer = IndexBuffer::Create(indices.data(), indices.size());
			vertex_array->SetIndexBuffer(index_buffer);
		}

		model->AddMeshSegment(MeshSegment::Create("BuiltinSphere", { vertex_array, PrimitiveType::Triangle_Strip }, material));

		return model;
	}

	/* ����һ��Plane���͵����� */
	static SharedPtr<Model> CreatePlane(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinPlane");

		/* ���MeshSegment */
		{
			// todo: Plane vertices
			static constexpr float vertices[] = {
				// Position,          TexCoord,    EntityId
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
				 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
				 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
				-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
				-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
			};

			// Vertex Array
			SharedPtr<VertexBuffer> vertex_buffer = VertexBuffer::Create(vertices, sizeof(vertices));
			VertexBufferLayout vertex_buffer_layout = {
				{ "a_Position", BufferDataType::Float3 },
				{ "a_TexCoord", BufferDataType::Float2 },
				{ "a_EntityId", BufferDataType::Int}
			};
			vertex_buffer->SetLayout(vertex_buffer_layout);

			auto vertex_array = VertexArray::Create();
			vertex_array->Bind();
			vertex_array->AddVertexBuffer(vertex_buffer);

			model->AddMeshSegment(CreateSharedPtr<MeshSegment>("BuiltinPlane", vertex_array, material));
		}

		return model;
	}

	Model::Model(std::string path)
		: m_DebugName(ExtractFilename(path))
		, m_Path(std::move(path))
		, m_AABBMin(glm::vec3(std::numeric_limits<float>::max()))
		, m_AABBMax(glm::vec3(-std::numeric_limits<float>::max()))
	{
	}

	/* ���һ��MeshSegment��ģ�� */
	void Model::AddMeshSegment(const SharedPtr<MeshSegment>& mesh_segment)
	{
		m_MeshSegments.emplace_back(mesh_segment);

		/* ����AABB */
		m_AABBMin.x = std::min(m_AABBMin.x, mesh_segment->m_AABBMin.x);
		m_AABBMin.y = std::min(m_AABBMin.y, mesh_segment->m_AABBMin.y);
		m_AABBMin.z = std::min(m_AABBMin.z, mesh_segment->m_AABBMin.z);
		m_AABBMax.x = std::max(m_AABBMax.x, mesh_segment->m_AABBMax.x);
		m_AABBMax.y = std::max(m_AABBMax.y, mesh_segment->m_AABBMax.y);
		m_AABBMax.z = std::max(m_AABBMax.z, mesh_segment->m_AABBMax.z);
	}

	/* ��·������һ��ģ�� */
	SharedPtr<Model> Model::Create(const std::string& path)
	{
		/* ��ȡ�ļ���׺�� */
		std::string suffix = ExtractFileSuffix(path);
		std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

		/* ����Meshģ������ */
		if (suffix == "mesh")
		{
			SharedPtr<Model> model = CreateSharedPtr<Model>(path);

			/* ����mtl�ļ� */
			const std::string mtl_filepath = ReplaceFileSuffix(path, ".mtl");
			model->LoadMaterial(mtl_filepath);
			const auto& material_group = model->GetMaterialGroup();

			/* ֱ�Ӵ�mesh�ļ����� */
			std::ifstream in_mesh_file(path, std::ios::in | std::ios::binary);
			if (!in_mesh_file)
			{
				CORE_LOG_ERROR("Failed to load file: {}.", path);
				return nullptr;
			}

			/* ��ȡ��ģ������ */
			size_t mesh_segment_cnt;
			in_mesh_file.read((char*)&mesh_segment_cnt, sizeof(size_t));

			for (size_t mesh_segment_idx = 0; mesh_segment_idx < mesh_segment_cnt; ++mesh_segment_idx)
			{
				/* ��ģ��Name */
				size_t name_size;
				in_mesh_file.read((char*)&name_size, sizeof(size_t));
				std::string name;
				in_mesh_file.read(name.data(), name_size);
				
				/* Vertex Array */
				auto vertex_array = VertexArray::Create();
				vertex_array->Bind();

				/* �������� */
				uint32_t vertex_count;
				in_mesh_file.read((char*)&vertex_count, sizeof(uint32_t));

				/* ��ģ���������� */
				size_t index_count;
				in_mesh_file.read((char*)&index_count, sizeof(size_t));
				uint32_t* indices = new uint32_t[index_count];
				in_mesh_file.read((char*)indices, index_count * sizeof(uint32_t));

				/* Index Buffer */
				auto index_buffer = IndexBuffer::Create(indices, index_count);
				vertex_array->SetIndexBuffer(index_buffer);
				delete[] indices;

				/* VertexBuffer������ */
				size_t vertex_buffer_count;
				in_mesh_file.read((char*)&vertex_buffer_count, sizeof(size_t));

				/* VertexBuffer���� */
				for (size_t i = 0; i < vertex_buffer_count; ++i)
				{
					uint32_t stride;
					in_mesh_file.read((char*)&stride, sizeof(uint32_t));

					float* buffer_data = new float[vertex_count * stride];
					in_mesh_file.read((char*)buffer_data, vertex_count * stride * sizeof(float));

					/* VertexLayout */
					VertexBufferLayout vertex_buffer_layout;
					size_t element_cnt;
					in_mesh_file.read((char*)&element_cnt, sizeof(size_t));
					for (size_t element_idx = 0; element_idx < element_cnt; ++element_idx)
					{
						/* Name */
						size_t element_name_size;
						in_mesh_file.read((char*)&element_name_size, sizeof(size_t));
						std::string element_name;
						in_mesh_file.read(element_name.data(), element_name_size);

						/* Type */
						uint8_t element_type;
						in_mesh_file.read((char*)&element_type, sizeof(uint8_t));

						/* Offset */
						size_t element_offset;
						in_mesh_file.read((char*)&element_offset, sizeof(size_t));

						/* Normalized */
						bool element_normalized;
						in_mesh_file.read((char*)&element_normalized, sizeof(bool));

						vertex_buffer_layout.EmplaceElement({ element_name, static_cast<BufferDataType>(element_type), element_normalized });
					}
					
					/* Vertex Buffer */
					auto vertex_buffer = VertexBuffer::Create(buffer_data, vertex_count * stride * sizeof(float));
					vertex_buffer->SetLayout(vertex_buffer_layout);
					vertex_array->AddVertexBuffer(vertex_buffer);
					delete[] buffer_data;
				}

				/* Material */
				int material_idx;
				in_mesh_file.read((char*)&material_idx, sizeof(int));
				auto material = material_group->GetMaterialByIndex(material_idx);
				if (!material)
					material = Material::Error();

				/* AABB */
				glm::vec3 aabb_min, aabb_max;
				in_mesh_file.read((char*)&aabb_min, sizeof(glm::vec3));
				in_mesh_file.read((char*)&aabb_max, sizeof(glm::vec3));

				/* ����MeshSegment */
				auto mesh_segment = CreateSharedPtr<MeshSegment>(name, vertex_array, material);
				mesh_segment->SetAABB(aabb_min, aabb_max);

				model->AddMeshSegment(mesh_segment);
			}

			/* �ر��ļ� */
			in_mesh_file.close();

			return model;
		}
		else
		{
			/* todo: support other model formats */
			CORE_LOG_ERROR("Unsupported model format: {}.", path);
			ASSERT(false);
			return nullptr;
		}
	}

	/* �����ڽ�ģ�� */
	SharedPtr<Model> Model::Create(BuiltinModelType type, const SharedPtr<Material>& material)
	{
		switch (type)
		{
		case BuiltinModelType::Cube:   return CreateCube(material);
		case BuiltinModelType::Sphere: return CreateSphere(material);
		case BuiltinModelType::Plane:  return CreatePlane(material);
		}
		return nullptr;
	}

	/* ����ģ��ʱ�����ز��� */
	void Model::LoadMaterial(const std::string& path)
	{
		m_pMaterialGroup = CreateSharedPtr<MaterialGroup>();
		m_pMaterialGroup->Deserializer(path);
	}

	SkeletonModel::SkeletonModel(const std::string& path)
		: Model(path)
	{
	}

	/* ��·������һ������ģ�� */
	SharedPtr<SkeletonModel> SkeletonModel::Create(const std::string& path)
	{
		auto [basedir, basename] = ExtractFileBaseDirAndBaseName(path);

		/* todo: */
		return CreateSharedPtr<SkeletonModel>(basename);
	}
}
