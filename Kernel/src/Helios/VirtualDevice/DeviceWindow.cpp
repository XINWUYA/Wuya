#include "Pch.h"
#include "DeviceWindow.h"
#include "Helios/Renderer/Renderer.h"
#include "GraphicsAPI/OpenGL/OpenGLWindow.h"
#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalWindow.h"
#endif

namespace Helios
{
    /* 创建窗口 */
    UniquePtr<DeviceWindow> DeviceWindow::Create(const WindowDesc& desc)
    {
        switch (Renderer::CurrentAPI())
        {
        case RenderAPI::None:
            CORE_LOG_ERROR("RenderAPI can't be None!");
            return nullptr;
        case RenderAPI::OpenGL:
            return CreateUniquePtr<OpenGLWindow>(desc);
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