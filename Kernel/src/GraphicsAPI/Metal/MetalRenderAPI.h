#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Renderer/RenderAPI.h"
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

namespace Helios
{
    class MetalRenderAPI : public RenderAPI
    {
    public:
        ~MetalRenderAPI() override;

        void Init() override;
        void SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height) override;
        void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        void SetClearColor(const glm::vec4& color) override;
        void Clear() override;

        /* 应用光栅化状态 */
        void ApplyRasterState(RenderRasterState raster_state) override;

        /* 绘制调用 */
        void DrawIndexed(PrimitiveType type, const SharedPtr<DeviceVertexArray>& vertex_array, uint32_t index_count = 0, uint32_t index_offset = 0) override;
        void DrawArrays(PrimitiveType type, const SharedPtr<DeviceVertexArray>& vertex_array) override;

        void Flush() override;

        /* 呈现当前帧 */
        void Present();

        /* 压入DebugGroup */
        void PushDebugGroup(const char* name) override;
        void PopDebugGroup() override;

    /* Metal特有接口 */
        MTL::Device* GetDevice() const { return m_Device; }
        MTL::CommandQueue* GetCommandQueue() const { return m_CommandQueue; }
        MTL::CommandBuffer* GetCurrentCommandBuffer() const { return m_CurrentCommandBuffer; }
        MTL::RenderCommandEncoder* GetCurrentRenderEncoder() const { return m_CurrentRenderEncoder; }
        CA::MetalDrawable* GetCurrentDrawable() const { return m_CurrentDrawable; }

        /* 设置Metal图层和渲染通道描述符 */
        void SetMetalLayer(CA::MetalLayer* layer) { m_MetalLayer = layer; }
        void SetRenderPassDescriptor(MTL::RenderPassDescriptor* descriptor) { m_RenderPassDescriptor = descriptor; }
        CA::MetalLayer* GetMetalLayer() const { return m_MetalLayer; }
        MTL::RenderPassDescriptor* GetRenderPassDescriptor() const { return m_RenderPassDescriptor; }

        /* 渲染通道管理 */
        void BeginRenderPass(MTL::RenderPassDescriptor* renderPassDescriptor);
        void EndRenderPass();

        /* 准备下一帧的drawable */
        void PrepareNextDrawable();

        /* 获取当前渲染目标尺寸 */
        uint32_t GetCurrentWidth() const { return m_CurrentWidth; }
        uint32_t GetCurrentHeight() const { return m_CurrentHeight; }

    private:
        /* Metal设备 */
        MTL::Device* m_Device{ nullptr };
        MTL::CommandQueue* m_CommandQueue{ nullptr };

        /* Metal图层和渲染通道 */
        CA::MetalLayer* m_MetalLayer{ nullptr };
        MTL::RenderPassDescriptor* m_RenderPassDescriptor{ nullptr };

        /* 当前渲染状态 */
        MTL::CommandBuffer* m_CurrentCommandBuffer{ nullptr };
        MTL::RenderCommandEncoder* m_CurrentRenderEncoder{ nullptr };
        CA::MetalDrawable* m_CurrentDrawable{ nullptr };

        /* 视口和清除颜色 */
        uint32_t m_CurrentWidth{ 0 };
        uint32_t m_CurrentHeight{ 0 };
        glm::vec4 m_ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };

        /* 光栅化状态 */
        RenderRasterState m_CurrentRasterState{};
    };
}

#endif /* PLATFORM_MACOS */
