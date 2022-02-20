#pragma once
#include <Wuya/Core/Layer.h>

namespace Wuya
{
	class ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttached() override;
		virtual void OnDetached() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(IEvent* event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_IsBlockEvents = block; }
		
	private:
		void SetDarkThemeColors();

		bool m_IsBlockEvents = true;
	};
}
