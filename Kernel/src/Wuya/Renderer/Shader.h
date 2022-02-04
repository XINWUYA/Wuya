#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	class IShader
	{
	public:
		IShader() = default;
		~IShader() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual const std::string& GetName() const = 0;

		static SharedPtr<IShader> Create(const std::string& filepath);
		static SharedPtr<IShader> Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src);
	};

	class ShaderLibrary
	{
	public:
		SharedPtr<IShader> Load(const std::string& filepath);
		SharedPtr<IShader> Load(const std::string& name, const std::string& filepath);

		SharedPtr<IShader> GetShaderByName(const std::string& name);
		bool IsExists(const std::string& name) const;

	private:
		void AddShader(const SharedPtr<IShader>& shader);
		void AddShader(const std::string& name, const SharedPtr<IShader>& shader);

		std::unordered_map<std::string, SharedPtr<IShader>> m_Shaders;
	};
}

