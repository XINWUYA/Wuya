#pragma once
#include <cstdint>
#include "Wuya/Common/Utils.h"

namespace Wuya
{
	/* Bufferʹ�÷�ʽ */
	enum class BufferUsage : uint8_t
	{
		Static,
		Dynamic,
		Stream
	};

	/* ���޳���ʽ */
	enum class CullMode : uint8_t
	{
		Cull_None,
		Cull_Front,
		Cull_Back,
		Cull_Front_And_Back
	};

	/* �������� */
	enum class FrontFaceType : uint8_t
	{
		CW, /* ˳ʱ��Ϊ���� */
		CCW /* ��ʱ��Ϊ���� */
	};

	/* ���ظ�ʽ���� */
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
		Alpha, /* һ��float */
	};

	/* �����ʽ */
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

	/* ����ʹ�÷�ʽ */
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

	/* ���������ʽ */
	enum class SamplerType : uint8_t
	{
		Sampler2D,
		Sampler2DArray,
		SamplerCubeMap,
		Sampler3D
	};

	/* ����������Ʒ�ʽ */
	enum class SamplerWrapMode : uint8_t
	{
		ClampToEdge,
		Repeat,
		MirroredRepeat
	};

	/* ������С���˷�ʽ */
	enum class SamplerMinFilter : uint8_t
	{
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear
	};

	/* ����Ŵ���˷�ʽ */
	enum class SamplerMagFilter : uint8_t
	{
		Nearest,
		Linear
	};

	/* BlendEquation */
	enum class BlendEquation : uint8_t
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	/* ��Ϸ�ʽ */
	enum class BlendFunc : uint8_t
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

	/* �ȽϷ�ʽ */
	enum class CompareFunc : uint8_t
	{
		LessEqual,		/* <= */
		GreaterEqual,	/* >= */
		Less,			/* < */
		Greater,		/* > */
		Equal,			/* = */
		NotEqual,		/* != */
		Always,			/* Always */
		Never,			/* Never */
	};

	/* ���ڱ��RenderBuffer��ʹ����� */
	enum class RenderBufferUsage : uint32_t
	{
		None		 = 0x00000000u,
		Color0		 = 0x00000001u,
		Color1		 = 0x00000002u,
		Color2		 = 0x00000004u,
		Color3		 = 0x00000008u,
		Color4		 = 0x00000010u,
		Color5		 = 0x00000020u,
		Color6		 = 0x00000040u,
		Color7		 = 0x00000080u,

		ColorDefault = Color0,
		ColorAll	 = Color0 | Color1 | Color2 | Color3 | Color4 | Color5 | Color6 | Color7,
		Depth		 = 0x10000000u,
		Stencil		 = 0x20000000u,
		DepthStencil = Depth | Stencil,
		All			 = ColorAll | DepthStencil
	};

	/* ����������ȡRenderBufferUsage */
	inline constexpr RenderBufferUsage GetRenderBufferUsageByIndex(uint32_t idx) noexcept
	{
		if (idx == 0u) return RenderBufferUsage::Color0;
		if (idx == 1u) return RenderBufferUsage::Color1;
		if (idx == 2u) return RenderBufferUsage::Color2;
		if (idx == 3u) return RenderBufferUsage::Color3;
		if (idx == 4u) return RenderBufferUsage::Color4;
		if (idx == 5u) return RenderBufferUsage::Color5;
		if (idx == 6u) return RenderBufferUsage::Color6;
		if (idx == 7u) return RenderBufferUsage::Color7;
		if (idx == 8u) return RenderBufferUsage::Depth;
		if (idx == 9u) return RenderBufferUsage::Stencil;
		return RenderBufferUsage::None;
	}

	/* ֧��λ���� */
	template<>
	struct EnableBitmaskOperators<TextureUsage> : public std::true_type {};
	template<>
	struct EnableBitmaskOperators<RenderBufferUsage> : public std::true_type {};

	/* ���� */
	static constexpr uint32_t MAX_COLOR_ATTACHMENT_NUM = 8;
	static constexpr uint32_t MAX_TEXTURE_UNIT_NUM = 32;

	/* �ӿ�������Ϣ */
	struct ViewportRegion
	{
		int32_t MinX;	/* ˮƽ������Сֵ */
		int32_t MinY;	/* ��ֱ������Сֵ */
		int32_t Width;	/* ���ֵ */
		int32_t Height;	/* �߶�ֵ */

		/* ����ˮƽ���ֵ����ֱ���ֵ */
		uint32_t MaxX() const { return MinX + Width; }
		uint32_t MaxY() const { return MinY + Height; }

		/* �еȲ��� */
		bool operator==(const ViewportRegion& other) noexcept
		{
			return MinX == other.MinX && MinY == other.MinY && Width == other.Width && Height == other.Height;
		}
		bool operator!=(const ViewportRegion& other) noexcept
		{
			return !(*this == other);
		}
	};

	/* ִ��һ��DrawCallʱ��RenderRasterState */
	struct RenderRasterState
	{
		CullMode CullMode				: 2; /* ���޳���ʽ */
		FrontFaceType FrontFaceType		: 1; /* �ж����淽ʽ */

		bool EnableBlend				: 1; /* ���û��ģʽ */
		BlendEquation BlendEquationRGB	: 3; /* ��Ϸ���RGB */
		BlendEquation BlendEquationA	: 3; /* ��Ϸ���A */
		BlendFunc BlendFuncSrcRGB		: 4; /* ��Ϸ�ʽSrcRGB */
		BlendFunc BlendFuncSrcA			: 4; /* ��Ϸ�ʽSrcA */
		BlendFunc BlendFuncDstRGB		: 4; /* ��Ϸ�ʽDstRGB */
		BlendFunc BlendFuncDstA			: 4; /* ��Ϸ�ʽDstA */

		bool EnableDepthWrite			: 1; /* ����д����� */
		CompareFunc DepthCompareFunc	: 3; /* ��Ȳ��ԱȽϷ�ʽ */

		bool EnableColorWrite			: 1; /* ������ɫд�� */

		uint32_t padding				: 1;

		/* Ĭ�Ϲ��� */
		RenderRasterState()
			: CullMode(CullMode::Cull_Back), FrontFaceType(FrontFaceType::CCW)
			, EnableBlend(true), BlendEquationRGB(BlendEquation::Add), BlendEquationA(BlendEquation::Add)
			, BlendFuncSrcRGB(BlendFunc::One), BlendFuncSrcA(BlendFunc::One)
			, BlendFuncDstRGB(BlendFunc::Zero), BlendFuncDstA(BlendFunc::Zero)
			, EnableDepthWrite(true), DepthCompareFunc(CompareFunc::LessEqual)
			, EnableColorWrite(true), padding(0)
		{}
	};

}
