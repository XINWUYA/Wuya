#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	/* Shader�� */
	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		/* �� */
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		/* ����Uniform���� */
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		/* ��ȡShader�� */
		virtual const std::string& GetName() const = 0;

		/* ����Shader */
		static SharedPtr<Shader> Create(const std::string& filepath);
		static SharedPtr<Shader> Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src);
	};

	/* Shader�⣺
	 * ���ڹ���Shader
	 */
	class ShaderLibrary
	{
	public:
		/* ���� */
		static ShaderLibrary& Instance();

		/* ���ļ��м���Shader */
		SharedPtr<Shader> GetOrLoad(const std::string& filepath);
		/* ����Shader */
		SharedPtr<Shader> Load(const std::string& name, const std::string& filepath);

		/* �������ƻ�ȡShader */
		SharedPtr<Shader> GetShaderByName(const std::string& name);
		/* �ж�Shader�Ƿ���� */
		[[nodiscard]] bool IsExists(const std::string& name) const;

	private:
		/* ���Shader */
		void AddShader(const SharedPtr<Shader>& shader);
		void AddShader(const std::string& name, const SharedPtr<Shader>& shader);

		/* Shader����Shader��ӳ��<Name, SharedPtr<Shader>> */
		std::unordered_map<std::string, SharedPtr<Shader>> m_Shaders;
	};
}

