#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	/* Shader类 */
	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

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
		virtual const std::string& GetName() const = 0;

		/* 创建Shader */
		static SharedPtr<Shader> Create(const std::string& filepath);
		static SharedPtr<Shader> Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src);
	};

	/* Shader库：
	 * 用于管理Shader
	 */
	class ShaderLibrary
	{
	public:
		/* 单例 */
		static ShaderLibrary& Instance();

		/* 从文件中加载Shader */
		SharedPtr<Shader> GetOrLoad(const std::string& filepath);
		/* 加载Shader */
		SharedPtr<Shader> Load(const std::string& name, const std::string& filepath);

		/* 根据名称获取Shader */
		SharedPtr<Shader> GetShaderByName(const std::string& name);
		/* 判断Shader是否存在 */
		[[nodiscard]] bool IsExists(const std::string& name) const;

	private:
		/* 添加Shader */
		void AddShader(const SharedPtr<Shader>& shader);
		void AddShader(const std::string& name, const SharedPtr<Shader>& shader);

		/* Shader名到Shader的映射<Name, SharedPtr<Shader>> */
		std::unordered_map<std::string, SharedPtr<Shader>> m_Shaders;
	};
}

