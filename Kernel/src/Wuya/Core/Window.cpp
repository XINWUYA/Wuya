#include "Pch.h"
#include "Window.h"
#include "Wuya/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/GLWindow.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalWindow.h"
#endif

namespace Wuya
{
    /* 创建窗口 */
    UniquePtr<IWindow> IWindow::Create(const WindowDesc& desc)
    {
        switch (Renderer::CurrentAPI())
        {
        case RenderAPI::None:
            CORE_LOG_ERROR("RenderAPI can't be None!");
            return nullptr;
        case RenderAPI::OpenGL:
            return CreateUniquePtr<GLWindow>(desc);
#ifdef PLATFORM_MACOS
        case RenderAPI::Metal:
            return CreateUniquePtr<MetalWindow>(desc);
#endif
        default:
            CORE_LOG_ERROR("Unknown RenderAPI is unsupported!");
            return nullptr;
        }
    }
}