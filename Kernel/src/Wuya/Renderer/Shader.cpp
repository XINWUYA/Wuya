#include "Pch.h"
#include "Shader.h"
#include "Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLShader.h"

namespace Wuya
{
	/* ����Shader */
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

	/* ����Shader */
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

	/* ���� */
	ShaderLibrary& ShaderLibrary::Instance()
	{
		static ShaderLibrary instance;
		return instance;
	}

	/* ���ļ��м���Shader */
	SharedPtr<Shader> ShaderLibrary::GetOrLoad(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		AddShader(shader);
		return shader;
	}

	/* ���ļ��м���Shader */
	SharedPtr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		AddShader(name, shader);
		return shader;
	}

	/* �������ƻ�ȡShader */
	SharedPtr<Shader> ShaderLibrary::GetShaderByName(const std::string& name)
	{
		if (!IsExists(name))
		{
			ASSERT(false, "Shader not found!");
			return nullptr;
		}

		return m_Shaders[name];
	}

	/* �ж�Shader�Ƿ���� */
	bool ShaderLibrary::IsExists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

	/* ���Shader */
	void ShaderLibrary::AddShader(const SharedPtr<Shader>& shader)
	{
		auto& name = shader->GetName();
		AddShader(name, shader);
	}

	/* ���Shader */
	void ShaderLibrary::AddShader(const std::string& name, const SharedPtr<Shader>& shader)
	{
		ASSERT(!IsExists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}
}
