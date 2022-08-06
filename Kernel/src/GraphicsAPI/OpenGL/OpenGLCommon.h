#pragma once
#include <glad/glad.h>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	/* ���뵽OpenGL��Ӧ���������� */
	constexpr GLenum TranslateToOpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:				 return GL_R8;
		case TextureFormat::R8_SNorm:		 return GL_R8_SNORM; 
		case TextureFormat::R16F:			 return GL_R16F;
		case TextureFormat::RG8:			 return GL_RG8;
		case TextureFormat::RG8_SNorm:		 return GL_RG8_SNORM;
		case TextureFormat::RGB565:			 return GL_RGB565;
		case TextureFormat::RGB9_E5:		 return GL_RGB9_E5;
		case TextureFormat::RGBA4:			 return GL_RGBA4;
		case TextureFormat::Depth16:		 return GL_DEPTH_COMPONENT16;
		case TextureFormat::RGB8:			 return GL_RGB8;
		case TextureFormat::s_RGB8:			 return GL_SRGB8;
		case TextureFormat::RGB8_SNorm:		 return GL_RGB8_SNORM;
		case TextureFormat::Depth24:		 return GL_DEPTH_COMPONENT24;
		case TextureFormat::R32F:			 return GL_R32F;
		case TextureFormat::RG16F:			 return GL_RG16F;
		case TextureFormat::R11G11B10F:		 return GL_R11F_G11F_B10F;
		case TextureFormat::RGBA8:			 return GL_RGBA8;
		case TextureFormat::s_RGBA8:		 return GL_SRGB8_ALPHA8;
		case TextureFormat::RGBA8_SNorm:	 return GL_RGBA8_SNORM;
		case TextureFormat::R10G10B10A2:	 return GL_RGB10_A2;
		case TextureFormat::Depth32:		 return GL_DEPTH_COMPONENT32;
		case TextureFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		case TextureFormat::RGBA16F:		 return GL_RGBA16F;
		case TextureFormat::RGB32F:			 return GL_RGB32F;
		case TextureFormat::RGBA32F:		 return GL_RGBA32F;
#if defined(GL_EXT_texture_compression_s3tc)
		case TextureFormat::DXT1_RGB:		 return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		case TextureFormat::s_DXT1_RGB:		 return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;
		case TextureFormat::DXT1_RGBA:		 return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		case TextureFormat::s_DXT1_RGBA:	 return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
		case TextureFormat::DXT3_RGBA:		 return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case TextureFormat::s_DXT3_RGBA:	 return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
		case TextureFormat::DXT5_RGBA:		 return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		case TextureFormat::s_DXT5_RGBA:	 return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
#else
		case TextureFormat::DXT1_RGB:
		case TextureFormat::s_DXT1_RGB:
		case TextureFormat::DXT1_RGBA:
		case TextureFormat::s_DXT1_RGBA:
		case TextureFormat::DXT3_RGBA:
		case TextureFormat::s_DXT3_RGBA:
		case TextureFormat::DXT5_RGBA:
		case TextureFormat::s_DXT5_RGBA:
			return 0;
#endif
		}
		return 0;
	}

	/* ���뵽OpenGL��Ӧ�ıȽϷ�ʽ */
	constexpr GLenum TranslateToOpenGLCompareFunc(CompareFunc func)
	{
		switch (func)
		{
		case CompareFunc::LessEqual:	return GL_LEQUAL;
		case CompareFunc::GreaterEqual: return GL_GEQUAL;
		case CompareFunc::Less:			return GL_LESS;
		case CompareFunc::Greater:		return GL_GREATER;
		case CompareFunc::Equal:		return GL_EQUAL;
		case CompareFunc::NotEqual:		return GL_NOTEQUAL;
		case CompareFunc::Always:		return GL_ALWAYS;
		case CompareFunc::Never:		return GL_NEVER;
		}
		return 0;
	}


	/* ���뵽OpenGL��Ӧ�Ļ�Ϸ��� */
	constexpr GLenum TranslateToOpenGLBlendEquation(BlendEquation equation)
	{
		switch (equation)
		{
		case BlendEquation::Add:				return GL_FUNC_ADD;
		case BlendEquation::Subtract:			return GL_FUNC_SUBTRACT;
		case BlendEquation::ReverseSubtract:	return GL_FUNC_REVERSE_SUBTRACT;
		case BlendEquation::Min:				return GL_MIN;
		case BlendEquation::Max:				return GL_MAX;
		}
		return 0;
	}

	/* ���뵽OpenGL��Ӧ�Ļ�Ϸ�ʽ */
	constexpr GLenum TranslateToOpenGLBlendFunc(BlendFunc func)
	{
		switch (func)
		{
		case BlendFunc::Zero:				return GL_ZERO;
		case BlendFunc::One:				return GL_ONE;
		case BlendFunc::SrcColor:			return GL_SRC_COLOR;
		case BlendFunc::OneMinusSrcColor:	return GL_ONE_MINUS_SRC_COLOR;
		case BlendFunc::DstColor:			return GL_DST_COLOR;
		case BlendFunc::OneMinusDstColor:	return GL_ONE_MINUS_DST_COLOR;
		case BlendFunc::SrcAlpha:			return GL_SRC_ALPHA;
		case BlendFunc::OneMinusSrcAlpha:	return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFunc::DstAlpha:			return GL_DST_ALPHA;
		case BlendFunc::OneMinusDstAlpha:	return GL_ONE_MINUS_DST_ALPHA;
		case BlendFunc::SrcAlphaSaturate:	return GL_SRC_ALPHA_SATURATE;
		}
		return 0;
	}

	/* OpenGL ������ */
#if WUYA_DEBUG
	void CheckGLError(const char* func_name, uint32_t line) noexcept;
	void CheckGLFrameBufferStatus(GLenum target, const char* func_name, uint32_t line) noexcept;
	#define CHECK_GL_ERROR { CheckGLError(__func__, __LINE__); }
	#define CHECK_GL_FRAMEBUFFER_STATUS(target) { CheckGLFrameBufferStatus(target, __func__, __LINE__); }
#else
	#define CHECK_GL_ERROR
	#define CHECK_GL_FRAMEBUFFER_STATUS(target)
#endif

}
