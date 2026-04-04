#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalTexture.h"
#include "MetalCommon.h"
#include "MetalRenderAPI.h"
#include "Wuya/Renderer/Renderer.h"
#include "stb_image.h"

namespace Wuya
{
    MetalTexture::MetalTexture(const std::string& name, const TextureDesc& texture_desc)
        : Texture(name, texture_desc)
    {
        CreateTexture(texture_desc);
        CreateSamplerState(texture_desc);
    }

    MetalTexture::MetalTexture(const std::string& path, const TextureLoadConfig& load_config)
        : Texture("Texture", TextureDesc{})
    {
        LoadFromFile(path, load_config);
    }

    MetalTexture::~MetalTexture()
    {
        if (m_Texture)
            m_Texture->release();
        if (m_SamplerState)
            m_SamplerState->release();
    }

    void MetalTexture::Bind(uint32_t slot)
    {
        m_Slot = slot;
        /* Metal中纹理在绘制时绑定到渲染编码器 */
    }

    void MetalTexture::Unbind()
    {
        /* Metal中无需解绑 */
    }

    void MetalTexture::SetData(void* data, const PixelDesc& pixel_desc, uint32_t level,
        uint32_t offset_x, uint32_t offset_y, uint32_t offset_z)
    {
        if (!m_Texture || !data)
            return;

        MTL::Region region;
        region.origin.x = offset_x;
        region.origin.y = offset_y;
        region.origin.z = offset_z;
        region.size.width = m_TextureDesc.Width >> level;
        region.size.height = m_TextureDesc.Height >> level;
        region.size.depth = 1;

        uint32_t bytes_per_row = region.size.width * 4; /* 假设RGBA格式 */
        m_Texture->replaceRegion(region, level, 0, data, bytes_per_row, 0);
    }

    bool MetalTexture::operator==(const Texture& other) const
    {
        return m_Texture == dynamic_cast<const MetalTexture&>(other).m_Texture;
    }

    void MetalTexture::CreateTexture(const TextureDesc& desc)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
            return;

        /* 创建纹理描述符 */
        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setTextureType(MTL::TextureType2D);
        textureDescriptor->setPixelFormat(ToMetalPixelFormat(desc.Format));
        textureDescriptor->setWidth(desc.Width);
        textureDescriptor->setHeight(desc.Height);
        textureDescriptor->setMipmapLevelCount(desc.MipLevels);

        /* 设置使用标志 */
        MTL::TextureUsage usage = MTL::TextureUsageShaderRead;
        if (!!(desc.Usage & TextureUsage::ColorAttachment) || 
            !!(desc.Usage & TextureUsage::DepthAttachment) ||
            !!(desc.Usage & TextureUsage::StencilAttachment))
            usage |= MTL::TextureUsageRenderTarget;
        textureDescriptor->setUsage(usage);

        /* 设置存储模式 - 使用Managed模式允许CPU通过replaceRegion上传数据 */
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        textureDescriptor->setStorageMode(MTL::StorageModeShared);
#else
        textureDescriptor->setStorageMode(MTL::StorageModeManaged);
#endif

        /* 创建纹理 */
        m_Texture = device->newTexture(textureDescriptor);
        textureDescriptor->release();

        if (!m_Texture)
        {
            CORE_LOG_ERROR("Failed to create Metal texture!");
        }
    }

    void MetalTexture::CreateSamplerState(const TextureDesc& desc)
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
            return;

        /* 创建采样器描述符 */
        MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();

        /* 设置过滤模式 - 使用默认线性过滤 */
        samplerDescriptor->setMinFilter(MTL::SamplerMinMagFilterLinear);
        samplerDescriptor->setMagFilter(MTL::SamplerMinMagFilterLinear);
        samplerDescriptor->setMipFilter(MTL::SamplerMipFilterLinear);

        /* 设置寻址模式 - 使用默认ClampToEdge */
        MTL::SamplerAddressMode addressMode = MTL::SamplerAddressModeClampToEdge;
        samplerDescriptor->setSAddressMode(addressMode);
        samplerDescriptor->setTAddressMode(addressMode);
        samplerDescriptor->setRAddressMode(addressMode);

        /* 创建采样器状态 */
        m_SamplerState = device->newSamplerState(samplerDescriptor);
        samplerDescriptor->release();

        if (!m_SamplerState)
        {
            CORE_LOG_ERROR("Failed to create Metal sampler state!");
        }
    }

    void MetalTexture::LoadFromFile(const std::string& path, const TextureLoadConfig& load_config)
    {
        m_Path = path;

        /* 使用stb_image加载图片 */
        int width, height, channels;
        stbi_set_flip_vertically_on_load(load_config.IsFlipV ? 1 : 0);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!data)
        {
            CORE_LOG_ERROR("Failed to load texture: {}", path);
            m_IsLoaded = false;
            return;
        }

        /* 设置纹理描述 */
        m_TextureDesc.Width = width;
        m_TextureDesc.Height = height;
        m_TextureDesc.Format = TextureFormat::RGBA8;
        m_TextureDesc.MipLevels = 1;

        /* 创建纹理 */
        CreateTexture(m_TextureDesc);
        CreateSamplerState(m_TextureDesc);

        /* 上传纹理数据 */
        SetData(data, PixelDesc{}, 0);

        /* 释放图片数据 */
        stbi_image_free(data);

        m_IsLoaded = true;
        CORE_LOG_INFO("Texture loaded: {} ({}x{})", path, width, height);
    }

    MTL::PixelFormat MetalTexture::ToMetalPixelFormat(TextureFormat format)    {
        switch (format)
        {
        case TextureFormat::RGBA8:
            return MTL::PixelFormatRGBA8Unorm;
        case TextureFormat::RGBA16F:
            return MTL::PixelFormatRGBA16Float;
        case TextureFormat::RGBA32F:
            return MTL::PixelFormatRGBA32Float;
        case TextureFormat::R8:
            return MTL::PixelFormatR8Unorm;
        case TextureFormat::RG8:
            return MTL::PixelFormatRG8Unorm;
        case TextureFormat::Depth24Stencil8:
            return MTL::PixelFormatDepth24Unorm_Stencil8;
        case TextureFormat::Depth32:
            return MTL::PixelFormatDepth32Float;
        default:
            return MTL::PixelFormatRGBA8Unorm;
        }
    }
}

#endif /* PLATFORM_MACOS */
