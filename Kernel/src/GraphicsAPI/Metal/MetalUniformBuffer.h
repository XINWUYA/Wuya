#pragma once
#ifdef PLATFORM_MACOS

#include "Helios/VirtualDevice/DeviceUniformBuffer.h"
#include <Metal/Metal.hpp>

namespace Helios
{
    class MetalUniformBuffer : public DeviceUniformBuffer
    {
    public:
        MetalUniformBuffer(uint32_t size, uint32_t binding_point);
        ~MetalUniformBuffer() override;

        void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

        MTL::Buffer* GetMetalBuffer() const { return m_Buffer; }
        uint32_t GetBindingPoint() const { return m_BindingPoint; }

    private:
        MTL::Buffer* m_Buffer{ nullptr };
        uint32_t m_Size{ 0 };
        uint32_t m_BindingPoint{ 0 };
    };
}

#endif /* PLATFORM_MACOS */
