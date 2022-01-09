#include "pch.h"
#include "RenderContext.h"
#include "Platforms/OpenGL/GLContext.h"

namespace Wuya
{
	UniquePtr<IRenderContext> IRenderContext::Create(void* window)
	{
		return CreateUniquePtr<GLContext>(static_cast<GLFWwindow*>(window));
		// todo: other device
	}
}
