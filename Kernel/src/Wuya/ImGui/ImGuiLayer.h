#pragma once
#include "Wuya/Core/Layer.h"

namespace Wuya
{
	class ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttached() override;
		virtual void OnDetached() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(IEvent* event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_IsBlockEvents = block; }
		
	private:
		void SetDefaultStyle();
		void SetDarkThemeColors();

		bool m_IsBlockEvents = true;
	};
}
