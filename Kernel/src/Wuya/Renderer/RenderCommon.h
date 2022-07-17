#pragma once
#include <cstdint>

namespace Wuya
{
	/* Buffer使用方式 */
	enum class BufferUsage : uint8_t
	{
		Static,
		Dynamic,
		Stream
	};

	/* 面剔除方式 */
	enum class CullMode : uint8_t
	{
		Cull_None,
		Cull_Front,
		Cull_Back,
		Cull_Front_And_Back
	};

	/* 像素格式类型 */
	enum class PixelFormatType : uint8_t
	{
		R,
		R_Integer,
		RG,
		RG_Integer,
		RGB,
		RGB_Integer,
		RGBA,
		RGBA_Integer,
		Depth,
		Depth24_Stencil8, 
		Alpha, /* 一个float */
	};

	/* 纹理格式 */
	enum class TextureFormat : uint8_t
	{
		R8,
		R8_SNorm,
		R16F,
		RG8,
		RG8_SNorm,
		RGB565,
		RGB9_E5,
		RGBA4,
		Depth16,
		RGB8,
		s_RGB8,
		RGB8_SNorm,
		Depth24,
		R32F,
		RG16F,
		R11G11B10F,
		RGBA8,
		s_RGBA8,
		RGBA8_SNorm,
		R10G10B10A2,
		Depth32,
		Depth24Stencil8,
		RGBA16F,
		RGB32F,
		RGBA32F,
		DXT1_RGB,
		s_DXT1_RGB,
		DXT1_RGBA,
		s_DXT1_RGBA,
		DXT3_RGBA,
		s_DXT3_RGBA,
		DXT5_RGBA,
		s_DXT5_RGBA
	};

	/* 纹理使用方式 */
	enum class TextureUsage : uint8_t
	{
		None			  = 0x00,
		ColorAttachment   = 0x01,
		DepthAttachment   = 0x02,
		StencilAttachment = 0x04,
		Sampleable		  = 0x08,
		Uploadable		  = 0x10,
		Default			  = Sampleable | Uploadable
	};

	/* 纹理采样方式 */
	enum class SamplerType : uint8_t
	{
		Sampler2D,
		Sampler2DArray,
		SamplerCubeMap,
		Sampler3D
	};

	/* 纹理采样环绕方式 */
	enum class SamplerWrapMode : uint8_t
	{
		ClampToEdge,
		Repeat,
		MirroredRepeat
	};

	/* 纹理缩小过滤方式 */
	enum class SamplerMinFilter : uint8_t
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	/* 纹理放大过滤方式 */
	enum class SamplerMagFilter : uint8_t
	{
		Nearest,
		Linear
	};

	/* 混合方式 */
	enum class BlendType : uint8_t
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha,
		SrcAlphaSaturate
	};

	/* 视口区域信息 */
	struct ViewportRegion
	{
		int32_t Left; /* 水平方向最小值 */
		int32_t Right; /* 水平方向最大值 */
		int32_t Bottom; /* 数值方向最小值 */
		int32_t Top; /* 数值方向最大值 */

		/* 区域宽度和高度 */
		uint32_t Width() const { return std::abs(Right - Left); }
		uint32_t Height() const { return std::abs(Top - Bottom); }

		/* 判等操作 */
		bool operator==(const ViewportRegion& other) noexcept
		{
			return Left == other.Left && Right == other.Right && Top == other.Top && Bottom == other.Bottom;
		}
		bool operator!=(const ViewportRegion& other) noexcept
		{
			return !(*this == other);
		}
	};
}

namespace std
{
	inline std::string to_string(Wuya::TextureUsage usage) noexcept
	{
		return "";
	}
}
