#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Core/Window.h"
#include "Helios/VirtualDevice/DeviceContext.h"
#include <QuartzCore/CAMetalLayer.hpp>
#include <Metal/Metal.hpp>

struct GLFWwindow;

namespace Helios
{
    /* MetalContext类：管理Metal渲染上下文和交换链 */
    class MetalContext final : public DeviceContext
    {
    public:
        MetalContext(GLFWwindow* window);
        ~MetalContext() override;

        void Init() override;
        void SwapBuffers() override;

        /* Metal特有接口 */
        MTL::Device* GetDevice() const { return m_Device; }
        MTL::CommandQueue* GetCommandQueue() const { return m_CommandQueue; }
        CA::MetalLayer* GetMetalLayer() const { return m_MetalLayer; }
        MTL::RenderPassDescriptor* GetRenderPassDescriptor() const { return m_RenderPassDescriptor; }
        MTL::Drawable* GetCurrentDrawable() const { return m_CurrentDrawable; }

    private:
        void CreateMetalLayer(GLFWwindow* window);

        GLFWwindow* m_Window{ nullptr };
        MTL::Device* m_Device{ nullptr };
        MTL::CommandQueue* m_CommandQueue{ nullptr };
        CA::MetalLayer* m_MetalLayer{ nullptr };
        MTL::RenderPassDescriptor* m_RenderPassDescriptor{ nullptr };
        MTL::Drawable* m_CurrentDrawable{ nullptr };
        MTL::Texture* m_DepthTexture{ nullptr };
    };

    /* MetalWindow类：使用GLFWwindow创建Metal窗口 */
    class MetalWindow : public DeviceWindow
    {
    public:
        MetalWindow(const WindowDesc& desc);
        ~MetalWindow() override;

        void OnUpdate() override;
        [[nodiscard]] uint32_t GetWidth() const override { return m_WindowInfo.Descriptor.Width; }
        [[nodiscard]] uint32_t GetHeight() const override { return m_WindowInfo.Descriptor.Height; }
        [[nodiscard]] bool IsVSync() const override { return m_WindowInfo.Descriptor.IsVSync; }
        void SetVSync(bool enable) override;
        [[nodiscard]] void* GetNativeWindow() const override { return m_pGLFWWindow; }
        void SetEventCallback(const EventCallbackFunc& callback) override { m_WindowInfo.CallBackFunc = callback; }
        MetalContext* GetMetalContext() const { return m_pMetalContext.get(); }

    private:
        void Build(const WindowDesc& desc);
        void Destroy();

        struct WindowInfo
        {
            WindowDesc Descriptor;
            EventCallbackFunc CallBackFunc;
        };

        GLFWwindow* m_pGLFWWindow{ nullptr };
        WindowInfo m_WindowInfo{};
        UniquePtr<MetalContext> m_pMetalContext{ nullptr };
    };
}

#endif
