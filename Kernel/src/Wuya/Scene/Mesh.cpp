#include "Pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Wuya/Renderer/Buffer.h"
#include "Wuya/Renderer/VertexArray.h"
#include <tiny_obj_loader.h>

#include "Wuya/Renderer/Shader.h"

namespace Wuya
{
	MeshSegment::MeshSegment(const SharedPtr<VertexArray>& vertex_array, const SharedPtr<Material>& material)
		: m_pVertexArray(vertex_array), m_pMaterial(material)
	{
	}

	/* 创建一个Cube类型的网格 */
	static SharedPtr<MeshSegment> CreateCube(const SharedPtr<Material>& material)
	{
		// Cube vertices
		static constexpr float vertices[] = {
			// Position,          TexCoord,    EntityId
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0,
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

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 创建一个Sphere类型的网格 */
	static SharedPtr<MeshSegment> CreateSphere(const SharedPtr<Material>& material)
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

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 创建一个Plane类型的网格 */
	static SharedPtr<MeshSegment> CreatePlane(const SharedPtr<Material>& material)
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

		return CreateSharedPtr<MeshSegment>(vertex_array, material);
	}

	/* 根据类型创建一个常规网格 */
	SharedPtr<MeshSegment> CreatePrimitive(PrimitiveType type, const SharedPtr<Material>& material)
	{
		switch (type)
		{
		case PrimitiveType::Cube:   return CreateCube(material);
		case PrimitiveType::Sphere: return CreateSphere(material);
		case PrimitiveType::Plane:  return CreatePlane(material);
		}
	}

	/* 从文件加载一个Obj模型 */
	std::vector<SharedPtr<MeshSegment>> LoadObjMeshFromFile(const std::string& filepath)
	{
		std::vector<SharedPtr<MeshSegment>> mesh_segments;

		tinyobj::attrib_t in_attribs; /* 包含所有的vertex、normal、texcoords */
		std::vector<tinyobj::shape_t> in_shapes; /* 包含每个网格数据段的indices、faces、material_idx */
		std::vector<tinyobj::material_t> in_materials; /* textures */

		/* 获取Obj文件所在目录 */
		auto GetRootDir = [](const std::string& path)->std::string
		{
			const auto pos = path.find_last_of("/\\");
			if (pos != std::string::npos)
				return path.substr(0, pos + 1);

			return "";
		};

		/* 加载模型数据 */
		std::string warn, err;
		bool ret = tinyobj::LoadObj(&in_attribs, &in_shapes, &in_materials, &warn, &err, filepath.c_str(), GetRootDir(filepath).c_str());

		if (!warn.empty())
			CORE_LOG_WARN("Load Obj Warning: {}.", warn);

		if (!err.empty())
			CORE_LOG_ERROR("Load Obj Error: {}.", err);

#if WUYA_DEBUG
		/* 打印模型信息 */
		CORE_LOG_DEBUG("Loading Obj: {}", filepath);
		CORE_LOG_DEBUG("Vertices  : {}", in_attribs.vertices.size() / 3);
		CORE_LOG_DEBUG("Normals   : {}", in_attribs.normals.size() / 3);
		CORE_LOG_DEBUG("Texcoords : {}", in_attribs.texcoords.size() / 2);
		CORE_LOG_DEBUG("Shapes    : {}", in_shapes.size());
		CORE_LOG_DEBUG("Materials : {}", in_materials.size());
#endif

		/* todo: 处理纹理 */

		/* 记录AABB信息 */
		glm::vec3 aabb_min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 aabb_max = glm::vec3(-std::numeric_limits<float>::max());

		for (size_t shape_idx = 0; shape_idx < in_shapes.size(); ++shape_idx)
		{
			/* 填充VertexData */
			std::vector<float> data;

			const auto& shape_data = in_shapes[shape_idx];
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
					vertex[0][i] = in_attribs.vertices[index0.vertex_index * 3 + i];
					vertex[1][i] = in_attribs.vertices[index1.vertex_index * 3 + i];
					vertex[2][i] = in_attribs.vertices[index2.vertex_index * 3 + i];

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
				if (!in_attribs.normals.empty())
				{
					if(index0.normal_index < 0 || index1.normal_index < 0 || index2.normal_index < 0)
					{
						invalid_normal = true;
					}
					else
					{
						for (int i = 0; i < 3; ++i)
						{
							/* x->y->z值依次获取 */
							normal[0][i] = in_attribs.normals[index0.normal_index * 3 + i];
							normal[1][i] = in_attribs.normals[index1.normal_index * 3 + i];
							normal[2][i] = in_attribs.normals[index2.normal_index * 3 + i];
						}
					}
				}

				if (invalid_normal)
				{
					/* todo: 生成法线 */
				}

				/* 收集3个顶点uv */
				float uv[3][2];
				memset(uv, 0.0f, 6 * sizeof(float));
				if (!in_attribs.texcoords.empty())
				{
					if (index0.texcoord_index >= 0 && index1.texcoord_index >= 0 && index2.texcoord_index >= 0)
					{
						uv[0][0] =	in_attribs.texcoords[index0.texcoord_index * 2];
						uv[0][1] = 1.0f - in_attribs.texcoords[index0.texcoord_index * 2 + 1];
						uv[1][0] = in_attribs.texcoords[index1.texcoord_index * 2];
						uv[1][1] = 1.0f - in_attribs.texcoords[index1.texcoord_index * 2 + 1];
						uv[2][0] = in_attribs.texcoords[index2.texcoord_index * 2];
						uv[2][1] = 1.0f - in_attribs.texcoords[index2.texcoord_index * 2 + 1];
					}
				}

				/* 收集3个顶点颜色 */

				/* todo: 收集 material */

				for (int v = 0; v < 3; ++v)
				{
					data.emplace_back(vertex[v][0]);
					data.emplace_back(vertex[v][1]);
					data.emplace_back(vertex[v][2]);
					data.emplace_back(normal[v][0]);
					data.emplace_back(normal[v][1]);
					data.emplace_back(normal[v][2]);
					data.emplace_back(uv[v][0]);
					data.emplace_back(uv[v][1]);
				}


			}

			/* Vertex Buffer */
			auto vertex_buffer = VertexBuffer::Create(data.data(), data.size() * sizeof(float));
			VertexBufferLayout vertex_buffer_layout = {
				{ "a_Position", BufferDataType::Float3 },
				{ "a_Normal", BufferDataType::Float3 },
				{ "a_TexCoord", BufferDataType::Float2 },
			};
			vertex_buffer->SetLayout(vertex_buffer_layout);

			/* Vertex Array */
			auto vertex_array = VertexArray::Create();
			vertex_array->Bind();
			vertex_array->AddVertexBuffer(vertex_buffer);

			/* Material */
			const auto shader = ShaderLibrary::Instance().GetOrLoad("assets/shaders/simple.glsl");
			auto material = CreateSharedPtr<Material>();
			material->SetShader(shader);

			mesh_segments.emplace_back(CreateSharedPtr<MeshSegment>(vertex_array, material));
		}

		return mesh_segments;
	}
}
