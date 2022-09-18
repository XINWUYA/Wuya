#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	/* Shader基类 */
	class Shader
	{
	public:
		Shader(std::string path);
		virtual ~Shader() = default;

		/* 路径 */
		const std::string& GetPath() const { return m_Path; }

		/* 绑定 */
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		/* 设置Uniform参数 */
		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		/* 获取Shader名 */
		virtual const std::string& GetDebugName() const = 0;

		/* 创建Shader */
		static SharedPtr<Shader> Create(const std::string& filepath);
		static SharedPtr<Shader> Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src);

	protected:
		/* 文件路径 */
		std::string m_Path{};
	};

	/* Shader库：
	 * 单例类；
	 * 用于管理Shader
	 */
	class ShaderLibrary
	{
	public:
		/* 单例 */
		static ShaderLibrary& Instance();

		/* 从文件中加载Shader */
		SharedPtr<Shader> GetOrLoad(const std::string& filepath);

	private:
		/* Shader名Hash到Shader的映射<NameHash, SharedPtr<Shader>> */
		std::unordered_map<uint32_t, SharedPtr<Shader>> m_Shaders;
	};
}

