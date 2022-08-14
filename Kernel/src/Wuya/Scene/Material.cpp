#include "Pch.h"
#include "Material.h"
#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"

namespace Wuya
{
	Material::Material(const std::string& path)
		: m_Path(path)
	{
		// todo: Load from file
	}

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
		m_Textures.emplace(texture, slot);
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
			texture->Bind(texture_info.second);
		}
	}

	/* 解绑材质 */
	void Material::Unbind()
	{
		m_pShader->Unbind();
	}
}
