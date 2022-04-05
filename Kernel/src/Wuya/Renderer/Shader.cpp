#include "pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLShader.h"

namespace Wuya
{
	SharedPtr<Shader> Shader::Create(const std::string& filepath)
	{
		switch(Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLShader>(filepath);
		default: 
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<Shader> Shader::Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLShader>(name, vertex_src, pixel_src);
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	SharedPtr<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		AddShader(shader);
		return shader;
	}

	SharedPtr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		AddShader(name, shader);
		return shader;
	}

	SharedPtr<Shader> ShaderLibrary::GetShaderByName(const std::string& name)
	{
		CORE_ASSERT(IsExists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::IsExists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

	void ShaderLibrary::AddShader(const SharedPtr<Shader>& shader)
	{
		auto& name = shader->GetName();
		AddShader(name, shader);
	}

	void ShaderLibrary::AddShader(const std::string& name, const SharedPtr<Shader>& shader)
	{
		CORE_ASSERT(!IsExists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}
}
