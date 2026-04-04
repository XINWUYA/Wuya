#pragma once

#ifdef PLATFORM_MACOS

#include "Wuya/Renderer/Texture.h"
#include <Metal/Metal.hpp>

namespace Wuya
{
    class MetalTexture : public Texture
    {
    public:
        MetalTexture(const std::string& name, const TextureDesc& texture_desc);
        MetalTexture(const std::string& path, const TextureLoadConfig& load_config);
        ~MetalTexture() override;

        void Bind(uint32_t slot = 0) override;
        void Unbind() override;

        void SetData(void* data, const PixelDesc& pixel_desc, uint32_t level = 0,
            uint32_t offset_x = 0, uint32_t offset_y = 0, uint32_t offset_z = 0) override;
        
        const std::string& GetPath() const override { return m_Path; }
        uint32_t GetTextureID() const override { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(m_Texture)); }
        bool IsLoaded() const override { return m_IsLoaded; }

        bool operator==(const Texture& other) const override;

        /* Metal特有接口 */
        MTL::Texture* GetMetalTexture() const { return m_Texture; }
        MTL::SamplerState* GetSamplerState() const { return m_SamplerState; }

    private:
        void CreateTexture(const TextureDesc& desc);
        void CreateSamplerState(const TextureDesc& desc);
        void LoadFromFile(const std::string& path, const TextureLoadConfig& load_config);
        MTL::PixelFormat ToMetalPixelFormat(TextureFormat format);

        MTL::Texture* m_Texture{ nullptr };
        MTL::SamplerState* m_SamplerState{ nullptr };
        std::string m_Path;
        bool m_IsLoaded{ false };
        uint32_t m_Slot{ 0 };
    };
}

#endif /* PLATFORM_MACOS */
