#pragma once
#include "Wuya/Core/Layer.h"
#include <memory>

namespace Wuya
{
	class ImGuiRenderer;
	class FrameGraph;

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
		/* 完成UI构建，生成DrawData（供ImGuiPass在FrameGraph中消费） */
		void PrepareRenderData();
		/* 多视口副窗口渲染（PlatformIO默认实现） */
		void RenderPlatformWindows();

		/* 将ImGui渲染作为一个Pass注入目标FrameGraph。
		 * 该Pass不会创建离屏FrameBuffer，而是直接将ImGui的DrawData渲染到当前
		 * 主窗口的默认RenderTarget上（Metal的drawable / OpenGL的default FBO） */
		void AddFrameGraphPass(FrameGraph& frame_graph);

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
