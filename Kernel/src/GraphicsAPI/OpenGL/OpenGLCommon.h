#pragma once
#include <glad/glad.h>
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	/* 翻译到OpenGL对应的纹理类型 */
	constexpr GLenum TranslateToOpenGLTextureFormat(TextureFormat format)
	{
		switch (format) {
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
}
