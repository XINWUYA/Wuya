#include "Pch.h"
#include "Material.h"
#include <tinyxml2.h>
#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	/* 默认材质和错误材质 */
	SharedPtr<Material> Material::m_pDefaultMaterial = CreateSharedPtr<Material>();// Material::Create();
	SharedPtr<Material> Material::m_pErrorMaterial = CreateSharedPtr<Material>();

	Material::~Material()
	{
		m_pShader.reset();
		m_Parameters.clear();
		m_Textures.clear();
	}

	void Material::SetParameters(const std::string& name, const std::any& param)
	{
		m_Parameters[name] = param;
	}

	void Material::SetTexture(const SharedPtr<Texture>& texture, uint32_t slot)
	{
		if (!texture)
			return;

		/* 若当前slot，已存在纹理， 则删除前一个纹理 */
		auto iter = m_Textures.begin();
		for (; iter != m_Textures.end(); ++iter)
		{
			if (iter->second == slot)
			{
				m_Textures.erase(iter);
				break;
			}
		}

		m_Textures.emplace(texture, slot);
	}

	/* 获取指定Slot上的Texture */
	const SharedPtr<Texture>& Material::GetTextureBySlot(uint32_t slot) const
	{
		for (const auto& item : m_Textures)
		{
			if (item.second == slot)
				return item.first;
		}
		return {};
	}

	/* 绑定材质中的各参数 */
	void Material::Bind()
	{
		/* 先绑定Shader */
		m_pShader->Bind();

		/* 绑定Uniform参数 */
		for (auto& param : m_Parameters)
		{
			auto& value = param.second;
			if (value.has_value())
			{
				auto& value_type = value.type();
				if (value_type == typeid(int))
				{
					m_pShader->SetInt(param.first, std::any_cast<int>(value));
				}
				else if (value_type == typeid(float))
				{
					m_pShader->SetFloat(param.first, std::any_cast<float>(value));
				}
				else if (value_type == typeid(glm::vec2))
				{
					m_pShader->SetFloat2(param.first, std::any_cast<glm::vec2>(value));
				}
				else if (value_type == typeid(glm::vec3))
				{
					m_pShader->SetFloat3(param.first, std::any_cast<glm::vec3>(value));
				}
				else if (value_type == typeid(glm::vec3))
				{
					m_pShader->SetFloat3(param.first, std::any_cast<glm::vec3>(value));
				}
				else if (value_type == typeid(glm::vec4))
				{
					m_pShader->SetFloat4(param.first, std::any_cast<glm::vec4>(value));
				}
				else if (value_type == typeid(glm::mat4))
				{
					m_pShader->SetMat4(param.first, std::any_cast<glm::mat4>(value));
				}
				/*else if (value_type == typeid({int*, uint32_t}))
				{
					// todo: IntArray
				}*/
				else
				{
					CORE_LOG_ERROR("Unsupported paramter type: {}.", value_type.name());
				}
			}
		}

		/* 绑定纹理 */
		for(auto& texture_info: m_Textures)
		{
			const auto& texture = texture_info.first;
			texture->Bind((uint32_t)texture_info.second);
		}
	}

	/* 解绑材质 */
	void Material::Unbind()
	{
		m_pShader->Unbind();
	}

	/* 默认材质 */
	SharedPtr<Material>& Material::Default()
	{
		if (!m_pDefaultMaterial->GetShader())
		{
			/* 只能在获取时设置Shader，在静态编译期，OpenGL尚未初始化，无法正确创建ShaderProgram */
			m_pDefaultMaterial->SetShader(ShaderLibrary::Instance().GetOrLoad("assets/shaders/default.glsl"));
		}

		return m_pDefaultMaterial;
	}

	/* 错误材质 */
	SharedPtr<Material>& Material::Error()
	{
		if (!m_pErrorMaterial->GetShader())
		{
			m_pErrorMaterial->SetShader(ShaderLibrary::Instance().GetOrLoad("assets/shaders/error.glsl"));
		}
		return m_pErrorMaterial;
	}
	
	/* 创建材质 */
	SharedPtr<Material> Material::Create(const SharedPtr<Shader>& shader)
	{
		auto material = CreateSharedPtr<Material>();
		material->SetShader(shader);

		return material;
	}

	MaterialGroup::MaterialGroup(std::string path)
		: m_Path(std::move(path))
	{
		Deserializer();
	}

	/* 添加一个Material */
	void MaterialGroup::EmplaceMaterial(const SharedPtr<Material>& material)
	{
		m_Materials.emplace_back(material);
	}

	/* 根据索引获取Material */
	const SharedPtr<Material>& MaterialGroup::GetMaterialByIndex(int idx)
	{
		if (idx < 0 || idx >= m_Materials.size())
		{
			CORE_LOG_ERROR("Unaccessable material idx.");
			return Material::Error();
		}

		return m_Materials[idx];
	}

	/* 反序列化 */
	bool MaterialGroup::Deserializer()
	{
		ASSERT(!m_Path.empty());

		auto* in_mtl_file = new tinyxml2::XMLDocument();
		tinyxml2::XMLError error = in_mtl_file->LoadFile(m_Path.c_str());
		if (error != tinyxml2::XML_SUCCESS)
		{
			CORE_LOG_ERROR("Failed to deserializer mtl file: {}.", m_Path);
			return false;
		}

		tinyxml2::XMLElement* mtl_root = in_mtl_file->FirstChildElement("Materials");
		auto* count = mtl_root->FindAttribute("Count");
		m_Materials.resize(count->IntValue());

		constexpr TextureLoadConfig texture_load_config = { false };
		for (tinyxml2::XMLElement* mtl_inst = mtl_root->FirstChildElement(); mtl_inst; mtl_inst = mtl_inst->NextSiblingElement("Material"))
		{
			auto material = CreateSharedPtr<Material>();
			auto* id_attr = mtl_inst->FindAttribute("ID");

			if (auto* albedo_tex = mtl_inst->FindAttribute("AlbedoTex"))
				material->SetTexture(Texture::Create(albedo_tex->Value(), texture_load_config), TextureSlot::Albedo);
			if (auto* specular_tex = mtl_inst->FindAttribute("SpecularTex"))
				material->SetTexture(Texture::Create(specular_tex->Value(), texture_load_config), TextureSlot::Specular);
			if (auto* normal_tex = mtl_inst->FindAttribute("NormalTex"))
				material->SetTexture(Texture::Create(normal_tex->Value(), texture_load_config), TextureSlot::Normal);
			if (auto* roughness_tex = mtl_inst->FindAttribute("RoughnessTex"))
				material->SetTexture(Texture::Create(roughness_tex->Value(), texture_load_config), TextureSlot::Roughness);
			if (auto* metallic_tex = mtl_inst->FindAttribute("MetallicTex"))
				material->SetTexture(Texture::Create(metallic_tex->Value(), texture_load_config), TextureSlot::Metallic);
			if (auto* emissive_tex = mtl_inst->FindAttribute("EmissiveTex"))
				material->SetTexture(Texture::Create(emissive_tex->Value(), texture_load_config), TextureSlot::Emissive);

			material->SetParameters("Diffuse", ToVec3(mtl_inst->Attribute("Diffuse")));
			material->SetParameters("Specular", ToVec3(mtl_inst->Attribute("Specular")));

			material->SetShader(ShaderLibrary::Instance().GetOrLoad(mtl_inst->Attribute("Shader")));

			m_Materials[id_attr->IntValue()] = material;
		}

		delete in_mtl_file;
		return true;
	}
}
