#pragma once
#include "Wuya/Common/Utils.h"

namespace Wuya
{
	class WUYA_API Application 
	{
	public:
		Application() = default;
		virtual ~Application() {};

		virtual void Run() = 0;
	};

	// Implement in clients
	Application* CreateApplication();
}