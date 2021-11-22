#pragma once
#include <Wuya/Core/Layer.h>
#include <Wuya/Common/Utils.h>

namespace Wuya
{
	class WUYA_API ImguiLayer : public ILayer
	{
	public:
		ImguiLayer();
		~ImguiLayer() = default;

		virtual void OnAttached() override;
		virtual void OnDetached() override;
		virtual void OnUpdate(float delta_time) override;
		virtual void OnEvent(IEvent* event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_IsBlockEvents = block; }
		
	private:
		void SetDarkThemeColors();

		bool m_IsBlockEvents = true;
	};
}
