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

		/* ���MeshSegment */
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

	/* ���ļ�����һ��Objģ�� */
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
			/* ����ģ�� */
			ImportObj(filepath);
		}

		auto [basedir, basename] = ExtractFileBaseDirAndBaseName(filepath);

		SharedPtr<Model> model = CreateSharedPtr<Model>(basename);
		model->SetPath(filepath);

		/* �ȼ���mtl�ļ� */
		const std::string mtl_filepath = filepath + ".mtl";
		MaterialGroup material_group(mtl_filepath);

		/* ֱ�Ӵ�mesh�ļ����� */
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

			/* ����MeshSegment */
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

		/* ����Objģ�� */
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

	SkeletonModel::SkeletonModel(const std::string& name)
		: Model(name)
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
