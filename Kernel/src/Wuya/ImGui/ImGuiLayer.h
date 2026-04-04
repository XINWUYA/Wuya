#pragma once
#include "Wuya/Core/Layer.h"
#include <memory>

namespace Wuya
{
	class ImGuiRenderer;

	class ImGuiLayer : public ILayer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		virtual void OnAttached() override;
		virtual void OnDetached() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(IEvent* event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_IsBlockEvents = block; }
		
		/* 获取渲染器 */
		ImGuiRenderer* GetRenderer() const { return m_Renderer.get(); }
		
	private:
		void SetDefaultStyle();
		void SetDarkThemeColors();

		bool m_IsBlockEvents = false;
		std::unique_ptr<ImGuiRenderer> m_Renderer;
	};
}
