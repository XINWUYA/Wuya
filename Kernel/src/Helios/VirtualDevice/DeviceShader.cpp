#include "Pch.h"
#include "DeviceShader.h"
#include "Helios/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLShader.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalShader.h"
#endif
#include <regex>
#include <spirv_cross.hpp>

namespace Helios
{
	DeviceShader::DeviceShader(std::string path)
		: m_Path(std::move(path))
	{
	}

	/* 创建Shader */
	SharedPtr<DeviceShader> DeviceShader::Create(const std::string& filepath)
	{
		switch(Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLShader>(filepath);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalShader>(filepath);
#endif
		default: 
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	/* 创建Shader */
	SharedPtr<DeviceShader> DeviceShader::Create(const std::string& name, const std::string& vertex_src, const std::string& pixel_src)
	{
		switch (Renderer::CurrentAPI())
		{
		case RenderAPI::None:
			CORE_LOG_ERROR("RenderAPI can't be None!");
			return nullptr;
		case RenderAPI::OpenGL:
			return CreateSharedPtr<OpenGLShader>(name, vertex_src, pixel_src);
#ifdef PLATFORM_MACOS
		case RenderAPI::Metal:
			return CreateSharedPtr<MetalShader>(name, vertex_src, pixel_src);
#endif
		default:
			CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
			return nullptr;
		}
	}

	void DeviceShader::ReflectFromSPIRV(const std::vector<uint32_t>& spirv)
	{
		if (spirv.empty())
			return;

		try
		{
			m_Reflection.Clear();

			spirv_cross::Compiler compiler(spirv);
			const spirv_cross::ShaderResources resources = compiler.get_shader_resources();

			/* GLSL 风格的组合采样器：uniform sampler2D / samplerCube / sampler2DArray ... */
			for (const auto& resource : resources.sampled_images)
			{
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				std::string name = resource.name.empty() ? compiler.get_name(resource.id) : resource.name;
				if (!name.empty())
					m_Reflection.SamplerBindings[name] = binding;
			}

			/* 分离式纹理（texture2D + sampler），Vulkan 风格 Shader 会走这里 */
			for (const auto& resource : resources.separate_images)
			{
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				std::string name = resource.name.empty() ? compiler.get_name(resource.id) : resource.name;
				if (!name.empty())
					m_Reflection.SamplerBindings[name] = binding;
			}

			for (const auto& resource : resources.separate_samplers)
			{
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				std::string name = resource.name.empty() ? compiler.get_name(resource.id) : resource.name;
				if (!name.empty())
					m_Reflection.SamplerStateBindings[name] = binding;
			}

			/* Storage image（compute） */
			for (const auto& resource : resources.storage_images)
			{
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				std::string name = resource.name.empty() ? compiler.get_name(resource.id) : resource.name;
				if (!name.empty())
					m_Reflection.SamplerBindings[name] = binding;
			}
		}
		catch (const std::exception& e)
		{
			CORE_LOG_ERROR("SPIRV-Cross reflection failed: {}", e.what());
		}
	}

	void DeviceShader::ReflectFromGLSLSource(const std::string& source)
	{
		if (source.empty())
			return;

		/* 匹配： layout(binding = N) uniform <type> <name>[数组]? ;
		 * 例如： layout(binding = 9) uniform sampler2DArray u_ShadowMap; */
		static const std::regex sampler_regex(R"(layout\s*\(\s*binding\s*=\s*([0-9]+)\s*\)\s*uniform\s+(?:highp\s+|mediump\s+|lowp\s+)?([A-Za-z_][A-Za-z0-9_]*)\s+([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]*\])?\s*;)");

		for (auto it = std::sregex_iterator(source.begin(), source.end(), sampler_regex); it != std::sregex_iterator(); ++it)
		{
			const std::smatch& match = *it;
			const std::string type = match.str(2);
			const std::string name = match.str(3);

			/* 只关心纹理/采样器类资源，uniform block 的类型名不会以这些前缀开头 */
			const bool is_texture_like =
				type.rfind("sampler", 0) == 0 ||
				type.rfind("image", 0) == 0 ||
				type.rfind("texture", 0) == 0 ||
				type.rfind("isampler", 0) == 0 ||
				type.rfind("usampler", 0) == 0;
			if (!is_texture_like)
				continue;

			m_Reflection.SamplerBindings[name] = static_cast<uint32_t>(std::stoul(match.str(1)));
		}
	}
}
