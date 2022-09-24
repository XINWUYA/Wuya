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

	/* 从文件加载一个Obj模型 */
	static SharedPtr<Model> LoadObjModelFromFile(const std::string& filepath)
	{
		if (filepath.empty())
		{
			CORE_LOG_ERROR("Model filepath is empty.");
			return nullptr;
		}

		const std::string mesh_filepath = filepath + ".mesh";
		if (!std::filesystem::exists(mesh_filepath))
		{
			/* 导入模型 */
			ImportObj(filepath);
		}

		auto [basedir, basename] = ExtractFileBaseDirAndBaseName(filepath);

		SharedPtr<Model> model = CreateSharedPtr<Model>(basename);
		model->SetPath(filepath);

		/* 先加载mtl文件 */
		const std::string mtl_filepath = filepath + ".mtl";
		MaterialGroup material_group(mtl_filepath);

		/* 直接从mesh文件加载 */
		std::ifstream in_mesh_file(mesh_filepath, std::ios::in | std::ios::binary);
		if (!in_mesh_file)
		{
			CORE_LOG_ERROR("Failed to load file: {}.", mesh_filepath);
			return nullptr;
		}

		size_t mesh_segment_cnt;
		in_mesh_file.read((char*)&mesh_segment_cnt, sizeof(size_t));

		for (size_t mesh_segment_idx = 0; mesh_segment_idx < mesh_segment_cnt; ++mesh_segment_idx)
		{
			size_t name_size;
			in_mesh_file.read((char*)&name_size, sizeof(size_t));
			std::string name;
			in_mesh_file.read(name.data(), name_size);

			size_t vertex_data_size;
			in_mesh_file.read((char*)&vertex_data_size, sizeof(size_t));

			float* vertex_data = new float[vertex_data_size];
			in_mesh_file.read((char*)vertex_data, vertex_data_size * sizeof(float));

			/* Vertex Buffer */
			auto vertex_buffer = VertexBuffer::Create(vertex_data, vertex_data_size * sizeof(float));
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

			/* Material */
			int material_idx;
			in_mesh_file.read((char*)&material_idx, sizeof(int));
			auto material = material_group.GetMaterialByIndex(material_idx);
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
			delete[] vertex_data;
		}
		in_mesh_file.close();
		
		return model;
	}

	Model::Model(std::string name)
		: m_DebugName(std::move(name))
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

		/* 加载Obj模型 */
		if (suffix == "obj")
		{
			return LoadObjModelFromFile(path);
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

	SkeletonModel::SkeletonModel(const std::string& name)
		: Model(name)
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
