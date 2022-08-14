#include "Pch.h"
#include "Window.h"
#include "GraphicsAPI/OpenGL/GLWindow.h"

namespace Wuya
{
    /* ���������������� */
    UniquePtr<IWindow> IWindow::Create(const WindowDesc& desc)
    {
        return CreateUniquePtr<GLWindow>(desc);
    }
}