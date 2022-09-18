#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	/* Shader���� */
	class Shader
	{
	public:
		Shader(std::string path);
		virtual ~Shader() = default;

		/* ·�� */
		const std::string& GetPath() const { return m_Path; }

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
		virtual const std::string& GetDebugName() const = 0;

		/* ����Shader */
		static SharedPtr<Shader> Create(const std::string& filepath);
		static SharedPtr<Shader> Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src);

	protected:
		/* �ļ�·�� */
		std::string m_Path{};
	};

	/* Shader�⣺
	 * �����ࣻ
	 * ���ڹ���Shader
	 */
	class ShaderLibrary
	{
	public:
		/* ���� */
		static ShaderLibrary& Instance();

		/* ���ļ��м���Shader */
		SharedPtr<Shader> GetOrLoad(const std::string& filepath);

	private:
		/* Shader��Hash��Shader��ӳ��<NameHash, SharedPtr<Shader>> */
		std::unordered_map<uint32_t, SharedPtr<Shader>> m_Shaders;
	};
}

