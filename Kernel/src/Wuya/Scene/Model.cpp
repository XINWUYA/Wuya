#include "Pch.h"
#include "Model.h"
#include "Mesh.h"
#include "ModelImporter.h"
#include "Wuya/Renderer/VertexArray.h"
#include "Wuya/Renderer/Buffer.h"
#include "Wuya/Renderer/Texture.h"
#include <tiny_obj_loader.h>
#include <tinyxml2.h>

#include "Wuya/Renderer/Shader.h"

namespace Wuya
{
	/* 创建一个Cube类型的模型 */
	static SharedPtr<Model> CreateCube(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinCube");

		/* 填充MeshSegment */
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

	/* 创建一个Sphere类型的网格 */
	static SharedPtr<Model> CreateSphere(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinSphere");

		/* 填充MeshSegment */
		{
			// todo: Cube vertices
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

			model->AddMeshSegment(CreateSharedPtr<MeshSegment>("BuiltinSphere", vertex_array, material));
		}

		return model;
	}

	/* 创建一个Plane类型的网格 */
	static SharedPtr<Model> CreatePlane(const SharedPtr<Material>& material)
	{
		auto model = CreateSharedPtr<Model>("BuiltinPlane");

		/* 填充MeshSegment */
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

	/* 添加一个MeshSegment到模型 */
	void Model::AddMeshSegment(const SharedPtr<MeshSegment>& mesh_segment)
	{
		m_MeshSegments.emplace_back(mesh_segment);

		/* 更新AABB */
		m_AABBMin.x = std::min(m_AABBMin.x, mesh_segment->m_AABBMin.x);
		m_AABBMin.y = std::min(m_AABBMin.y, mesh_segment->m_AABBMin.y);
		m_AABBMin.z = std::min(m_AABBMin.z, mesh_segment->m_AABBMin.z);
		m_AABBMax.x = std::max(m_AABBMax.x, mesh_segment->m_AABBMax.x);
		m_AABBMax.y = std::max(m_AABBMax.y, mesh_segment->m_AABBMax.y);
		m_AABBMax.z = std::max(m_AABBMax.z, mesh_segment->m_AABBMax.z);
	}

	/* 从路径加载一个模型 */
	SharedPtr<Model> Model::Create(const std::string& path)
	{
		/* 获取文件后缀名 */
		std::string suffix = ExtractFileSuffix(path);
		std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

		/* 加载Mesh模型数据 */
		if (suffix == "mesh")
		{
			SharedPtr<Model> model = CreateSharedPtr<Model>(path);

			/* 加载mtl文件 */
			const std::string mtl_filepath = ReplaceFileSuffix(path, ".mtl");
			model->LoadMaterial(mtl_filepath);
			const auto& material_group = model->GetMaterialGroup();

			/* 直接从mesh文件加载 */
			std::ifstream in_mesh_file(path, std::ios::in | std::ios::binary);
			if (!in_mesh_file)
			{
				CORE_LOG_ERROR("Failed to load file: {}.", path);
				return nullptr;
			}

			/* 读取子模型数量 */
			size_t mesh_segment_cnt;
			in_mesh_file.read((char*)&mesh_segment_cnt, sizeof(size_t));

			for (size_t mesh_segment_idx = 0; mesh_segment_idx < mesh_segment_cnt; ++mesh_segment_idx)
			{
				/* 子模型Name */
				size_t name_size;
				in_mesh_file.read((char*)&name_size, sizeof(size_t));
				std::string name;
				in_mesh_file.read(name.data(), name_size);
				
				/* Vertex Array */
				auto vertex_array = VertexArray::Create();
				vertex_array->Bind();

				/* 顶点数量 */
				uint32_t vertex_count;
				in_mesh_file.read((char*)&vertex_count, sizeof(uint32_t));

				/* 子模型索引数据 */
				size_t index_count;
				in_mesh_file.read((char*)&index_count, sizeof(size_t));
				uint32_t* indices = new uint32_t[index_count];
				in_mesh_file.read((char*)indices, index_count * sizeof(uint32_t));

				/* Index Buffer */
				auto index_buffer = IndexBuffer::Create(indices, index_count);
				vertex_array->SetIndexBuffer(index_buffer);
				delete[] indices;

				/* VertexBuffer的数量 */
				size_t vertex_buffer_count;
				in_mesh_file.read((char*)&vertex_buffer_count, sizeof(size_t));

				/* VertexBuffer数据 */
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

				/* 创建MeshSegment */
				auto mesh_segment = CreateSharedPtr<MeshSegment>(name, vertex_array, material);
				mesh_segment->SetAABB(aabb_min, aabb_max);

				model->AddMeshSegment(mesh_segment);
			}

			/* 关闭文件 */
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

	/* 创建内建模型 */
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

	/* 加载模型时，加载材质 */
	void Model::LoadMaterial(const std::string& path)
	{
		m_pMaterialGroup = CreateSharedPtr<MaterialGroup>();
		m_pMaterialGroup->Deserializer(path);
	}

	SkeletonModel::SkeletonModel(const std::string& path)
		: Model(path)
	{
	}

	/* 从路径加载一个骨骼模型 */
	SharedPtr<SkeletonModel> SkeletonModel::Create(const std::string& path)
	{
		auto [basedir, basename] = ExtractFileBaseDirAndBaseName(path);

		/* todo: */
		return CreateSharedPtr<SkeletonModel>(basename);
	}
}
