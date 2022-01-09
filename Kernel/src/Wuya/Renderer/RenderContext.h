#pragma once

namespace Wuya
{
	class IRenderContext
	{
	public:
		virtual ~IRenderContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static UniquePtr<IRenderContext> Create(void* window);
	};

}