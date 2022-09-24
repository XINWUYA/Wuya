#pragma once
#include <glad/glad.h>
#include "Wuya/Renderer/Shader.h"

namespace Wuya
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(std::string name, const std::string& vertex_src, const std::string& pixel_src);
		virtual ~OpenGLShader();

		void Bind() override;
		void Unbind() override;
		void SetInt(const std::string& name, int value) override;
		void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const glm::vec2& value) override;
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetFloat4(const std::string& name, const glm::vec4& value) override;
		void SetMat4(const std::string& name, const glm::mat4& value) override;
		const std::string& GetDebugName() const override { return m_DebugName; }

	private:
		std::string ReadFile(const std::string& filepath);
		void PreProcessShaderSrc(const std::string& source, std::unordered_map<GLenum, std::string>& shader_sources /*<ShaderStage, ShaderSrc>*/);
		void CompileShadersToOpenGL();
		void CreateShaderProgram();

		std::string m_DebugName{"Unnamed Shader"};
		uint32_t m_ProgramID{ 0 };

		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRVs{};
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCodes{};
	};
}
