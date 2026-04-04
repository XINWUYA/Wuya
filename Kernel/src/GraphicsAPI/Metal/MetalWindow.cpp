#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalWindow.h"
#include "MetalRenderAPI.h"
#include <GLFW/glfw3.h>
#include <objc/message.h>
#include <objc/objc.h>
#include "Wuya/Events/ApplicationEvent.h"
#include "Wuya/Events/KeyEvent.h"
#include "Wuya/Events/MouseEvent.h"
#include "Wuya/Renderer/Renderer.h"

/* 前向声明GLFW native函数 */
extern "C" void* glfwGetCocoaWindow(GLFWwindow* window);

namespace Wuya
{
    /* MetalContext实现 */
    MetalContext::MetalContext(GLFWwindow* window)
        : m_Window(window)
    {
        ASSERT(window, "GLFWwindow is nullptr!");
    }

    MetalContext::~MetalContext()
    {
        if (m_RenderPassDescriptor)
            m_RenderPassDescriptor->release();
        if (m_DepthTexture)
            m_DepthTexture->release();
        if (m_CommandQueue)
            m_CommandQueue->release();
        if (m_Device)
            m_Device->release();
    }

    void MetalContext::Init()
    {
        CreateMetalLayer(m_Window);

        CORE_LOG_INFO("Using Metal:");
        CORE_LOG_INFO("    Device: {}", m_Device->name()->utf8String());

        /* 将MetalLayer和RenderPassDescriptor传递给MetalRenderAPI */
        auto metalRenderAPI = std::dynamic_pointer_cast<MetalRenderAPI>(Renderer::GetRenderAPI());
        if (metalRenderAPI)
        {
            metalRenderAPI->SetMetalLayer(m_MetalLayer);
            metalRenderAPI->SetRenderPassDescriptor(m_RenderPassDescriptor);
        }
    }

    void MetalContext::CreateMetalLayer(GLFWwindow* window)
    {
        /* 创建Metal设备 */
        m_Device = MTL::CreateSystemDefaultDevice();
        if (!m_Device)
        {
            CORE_LOG_ERROR("Failed to create Metal device!");
            return;
        }

        /* 创建命令队列 */
        m_CommandQueue = m_Device->newCommandQueue();

        /* 创建Metal图层 */
        m_MetalLayer = CA::MetalLayer::layer();
        m_MetalLayer->setDevice(m_Device);
        m_MetalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        m_MetalLayer->setFramebufferOnly(true);

        /* 获取NSWindow并设置Metal图层 - 使用Objective-C运行时 */
        id cocoaWindow = (id)glfwGetCocoaWindow(window);
        id contentView = ((id (*)(id, SEL))objc_msgSend)(cocoaWindow, sel_registerName("contentView"));
        ((void (*)(id, SEL, id))objc_msgSend)(contentView, sel_registerName("setLayer:"), m_MetalLayer);
        ((void (*)(id, SEL, BOOL))objc_msgSend)(contentView, sel_registerName("setWantsLayer:"), YES);

        /* 设置图层大小 */
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        m_MetalLayer->setDrawableSize({static_cast<float>(width), static_cast<float>(height)});

        /* 创建渲染通道描述符 */
        m_RenderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        /* 创建深度+模板纹理（使用Depth32Float_Stencil8格式，同时支持深度和模板） */
        auto depthAttachment = m_RenderPassDescriptor->depthAttachment();
        auto stencilAttachment = m_RenderPassDescriptor->stencilAttachment();
        MTL::TextureDescriptor* depthDesc = MTL::TextureDescriptor::texture2DDescriptor(
            MTL::PixelFormatDepth32Float_Stencil8, width, height, false);
        depthDesc->setStorageMode(MTL::StorageModePrivate);
        depthDesc->setUsage(MTL::TextureUsageRenderTarget);
        m_DepthTexture = m_Device->newTexture(depthDesc);
        
        /* 设置深度附件 */
        depthAttachment->setTexture(m_DepthTexture);
        depthAttachment->setLoadAction(MTL::LoadActionClear);
        depthAttachment->setStoreAction(MTL::StoreActionStore);
        depthAttachment->setClearDepth(1.0);
        
        /* 设置模板附件（与深度附件共享同一纹理） */
        stencilAttachment->setTexture(m_DepthTexture);
        stencilAttachment->setLoadAction(MTL::LoadActionClear);
        stencilAttachment->setStoreAction(MTL::StoreActionStore);
        stencilAttachment->setClearStencil(0);
        
        depthDesc->release();

        CORE_LOG_INFO("Metal layer created ({}x{})", width, height);
    }

    void MetalContext::SwapBuffers()
    {
        /* 提交命令缓冲区并呈现drawable */
        auto metalRenderAPI = std::dynamic_pointer_cast<MetalRenderAPI>(Renderer::GetRenderAPI());
        if (metalRenderAPI)
        {
            metalRenderAPI->Present();
        }
    }

    /* MetalWindow实现 */
    MetalWindow::MetalWindow(const WindowDesc& desc)
    {
        Build(desc);
    }

    MetalWindow::~MetalWindow()
    {
        Destroy();
    }

    void MetalWindow::OnUpdate()
    {
        glfwPollEvents();
        
        /* 提交命令缓冲区并交换缓冲区 */
        m_pMetalContext->SwapBuffers();
    }

    void MetalWindow::SetVSync(bool enable)
    {
        m_WindowInfo.Descriptor.IsVSync = enable;
        glfwSwapInterval(enable ? 1 : 0);
    }

    void MetalWindow::Build(const WindowDesc& desc)
    {
        /* 初始化GLFW */
        if (!glfwInit())
        {
            CORE_LOG_ERROR("Could not initialize GLFW!");
            return;
        }

        /* 配置GLFW不创建OpenGL上下文 */
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        /* 创建GLFW窗口 */
        m_pGLFWWindow = glfwCreateWindow(desc.Width, desc.Height, desc.Title.c_str(), nullptr, nullptr);
        if (!m_pGLFWWindow)
        {
            CORE_LOG_ERROR("Could not create GLFW window!");
            glfwTerminate();
            return;
        }

        glfwSetWindowUserPointer(m_pGLFWWindow, &m_WindowInfo);
        SetVSync(desc.IsVSync);

        /* 创建Metal上下文 */
        m_pMetalContext = CreateUniquePtr<MetalContext>(m_pGLFWWindow);
        m_pMetalContext->Init();

        /* 设置窗口大小回调 */
        glfwSetWindowSizeCallback(m_pGLFWWindow, [](GLFWwindow* window, int width, int height) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            info->Descriptor.Width = width;
            info->Descriptor.Height = height;
            WindowResizeEvent event(width, height);
            info->CallBackFunc(&event);
        });

        /* 设置窗口关闭回调 */
        glfwSetWindowCloseCallback(m_pGLFWWindow, [](GLFWwindow* window) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            info->CallBackFunc(&event);
        });

        /* 设置键盘回调 */
        glfwSetKeyCallback(m_pGLFWWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS: {
                    KeyPressedEvent event(key, 0);
                    info->CallBackFunc(&event);
                    break;
                }
                case GLFW_RELEASE: {
                    KeyReleasedEvent event(key);
                    info->CallBackFunc(&event);
                    break;
                }
                case GLFW_REPEAT: {
                    KeyPressedEvent event(key, 1);
                    info->CallBackFunc(&event);
                    break;
                }
            }
        });

        /* 设置鼠标按钮回调 */
        glfwSetMouseButtonCallback(m_pGLFWWindow, [](GLFWwindow* window, int button, int action, int mods) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            switch (action) {
                case GLFW_PRESS: {
                    MouseButtonPressedEvent event(button);
                    info->CallBackFunc(&event);
                    break;
                }
                case GLFW_RELEASE: {
                    MouseButtonReleasedEvent event(button);
                    info->CallBackFunc(&event);
                    break;
                }
            }
        });

        /* 设置滚轮回调 */
        glfwSetScrollCallback(m_pGLFWWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            info->CallBackFunc(&event);
        });

        /* 设置光标位置回调 */
        glfwSetCursorPosCallback(m_pGLFWWindow, [](GLFWwindow* window, double xPos, double yPos) {
            WindowInfo* info = (WindowInfo*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            info->CallBackFunc(&event);
        });

        m_WindowInfo.Descriptor = desc;
        CORE_LOG_INFO("Metal window created: {} ({}x{})", desc.Title, desc.Width, desc.Height);
    }

    void MetalWindow::Destroy()
    {
        glfwDestroyWindow(m_pGLFWWindow);
    }
}

#endif
