#include "pch.h"
#include "Window.h"
#include "Platforms/OpenGL/GLWindow.h"

namespace Wuya
{
    UniquePtr<IWindow> Wuya::IWindow::Create(const WindowConfig& config)
    {
        return CreateUniquePtr<GLWindow>(config);
    }
}