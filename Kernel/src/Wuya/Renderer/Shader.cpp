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
		const auto key = ToID(filepath);
		const auto iter = m_Shaders.find(key);

		/* ���������·���м��� */
		if (iter == m_Shaders.end())
		{
			auto shader = Shader::Create(filepath);
			m_Shaders[key] = shader;
			return shader;
		}

		return iter->second;
	}
}
