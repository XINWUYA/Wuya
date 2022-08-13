#pragma once
#include "FrameGraphResourceHandle.h"
#include "Wuya/Renderer/RenderCommon.h"

namespace Wuya
{
	class FrameGraph;
	class RenderPassNode;
	class IResource;
	class FrameBuffer;

	/* FrameGraph��һ��RenderPassNode��ʹ�õ���Դ */
	class FrameGraphResources
	{
	public:
		FrameGraphResources(FrameGraph& frame_graph, RenderPassNode& render_pass_node);

		/* ��ȡ��ǰRenderPassNode�� */
		const std::string& GetPassName() const;

		/* ��ȡָ����Դ */
		template<typename ResourceType>
		const ResourceType& Get(FrameGraphResourceHandleTyped<ResourceType> handle) const
		{
			return StaticPtrCast<const Resource<ResourceType>>(GetResource(handle))->GetResource();
		}

		/* ��ȡָ����Դ������ */
		template <typename ResourceType>
		const typename ResourceType::Descriptor& GetDescriptor(FrameGraphResourceHandleTyped<ResourceType> handle) const
		{
			return StaticPtrCast<const Resource<ResourceType>>(GetResource(handle))->GetDescriptor();
		}

		/* ��ȡָ����Դ����; */
		template <typename ResourceType>
		const typename ResourceType::Usage& GetUsage(FrameGraphResourceHandleTyped<ResourceType> handle) const
		{
			return StaticPtrCast<const Resource<ResourceType>>(GetResource(handle))->GetUsage();
		}

		/* ��ȡ��ǰRenderPass��RenderTarget */
		SharedPtr<FrameBuffer> GetPassRenderTarget(uint32_t idx = 0) const;

	private:
		/* ��FrameGraph�л�ȡ��Դ */
		const SharedPtr<IResource>& GetResource(FrameGraphResourceHandle handle) const;

		/* ����FrameGraph */
		FrameGraph& m_FrameGraph;
		/* ������FrameGraphPass */
		RenderPassNode& m_RenderPassNode;
	};

	class Texture;

	/* FrameGraph��ʹ�õ�Texture */
	struct FrameGraphTexture
	{
		SharedPtr<Texture> Texture;

		/* ���� */
		struct Descriptor
		{
			uint32_t Width{ 1 };
			uint32_t Height{ 1 }; 
			uint32_t Depth{ 1 }; /* �ɱ�ʾ3D���� */
			uint8_t  MipLevels{ 1 }; /* ���ɵ�Mip�㼶�� */
			uint8_t  Samples{ 0 }; /* 0���Զ�������1�����������ز�����>1�����ɲ��� */

			TextureFormat TextureFormat{ TextureFormat::RGBA8 };
			SamplerType   SamplerType{ SamplerType::Sampler2D };
		};

		/* ����Դ���� */
		struct SubDescriptor
		{
			/* Mip level */
			uint16_t Level{ 0 };
			/* Face or slice */
			uint16_t Layer{ 0 };
		};

		/* ʹ�÷�ʽ */
		using Usage = TextureUsage;
		static constexpr Usage DefaultReadUsage = TextureUsage::Sampleable;
		static constexpr Usage DefaultWriteUsage = TextureUsage::ColorAttachment;

		/* ���� */
		void Create(const std::string& name, const Descriptor& desc, Usage usage);
		/* ���� */
		void Destroy();
	};
}
