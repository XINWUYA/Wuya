#pragma once
#include "RenderAPI.h"

namespace Wuya
{
	class Renderer
	{
	public:
		static int GetAPI() { return RenderAPI::GetAPI(); }
	};
}
