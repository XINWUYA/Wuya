#include "Pch.h"
#include "Window.h"
#include "GraphicsAPI/OpenGL/GLWindow.h"

namespace Wuya
{
    /* 根据描述创建窗口 */
    UniquePtr<IWindow> IWindow::Create(const WindowDesc& desc)
    {
        return CreateUniquePtr<GLWindow>(desc);
    }
}