#pragma once
#include <Wuya/Core/Layer.h>

namespace Wuya
{
	class ImguiLayer : public ILayer
	{
	public:
		ImguiLayer();
		~ImguiLayer() = default;

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
