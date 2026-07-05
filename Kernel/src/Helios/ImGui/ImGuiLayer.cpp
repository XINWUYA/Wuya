#include "Pch.h"
#include "ImGuiLayer.h"
#include "ImGuiRenderer.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include "Helios/Application/Application.h"
#include "Helios/Core/Window.h"
#include "Helios/Renderer/Renderer.h"
#include "Helios/Renderer/RenderAPI.h"
#include "Helios/Renderer/FrameGraph/FrameGraph.h"
#include "Helios/Scene/SceneCommon.h"

#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalRenderAPI.h"
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#endif

namespace Helios
{
	/* ImGuiPass Payload：此Pass不依赖任何FrameGraph资源 */
	struct ImGuiPassData
	{};

	ImGuiLayer::ImGuiLayer()
		: ILayer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttached()
	{
		PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;			// Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Set ini file path to absolute path for consistent layout persistence
		// Use the same directory as ASSETS_PATH (project root)
		const static std::string ini_path = std::string(ASSETS_PATH) + "/../imgui.ini";
		io.IniFilename = ini_path.c_str();

		const float fontSize = 18.0f;
		if (!io.Fonts->AddFontFromFileTTF(ABSOLUTE_PATH("EditorRes/fonts/msyh.ttf").c_str(), fontSize))
		{
			CORE_LOG_ERROR("Failed to load font: EditorRes/fonts/msyh.ttf");
		}

		// Setup Dear ImGui style
		SetDefaultStyle();
		SetDarkThemeColors();

		// Setup Platform bindings (GLFW only, no renderer backend)
		auto* window = static_cast<GLFWwindow*>(Application::Instance()->GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);

		// Create our custom renderer
		m_Renderer = std::make_unique<ImGuiRenderer>();
		m_Renderer->Init();

		CORE_LOG_INFO("ImGuiLayer initialized with custom ImGuiRenderer");
	}

	void ImGuiLayer::OnDetached()
	{
		PROFILE_FUNCTION();

		m_Renderer->Cleanup();
		m_Renderer.reset();

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(IEvent* event)
	{
		if (m_IsBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event->Handled |= event->IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event->Handled |= event->IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		// Show Demo
		//bool show = true;
		//ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::Begin()
	{
		PROFILE_FUNCTION();

		// Update display size
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(
			(float)Application::Instance()->GetWindow().GetWidth(),
			(float)Application::Instance()->GetWindow().GetHeight()
		);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		// New frame for renderer
		m_Renderer->NewFrame(1.0f / 60.0f);

		// Platform new frame
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::PrepareRenderData()
	{
		PROFILE_FUNCTION();

		/* 仅生成DrawData，实际提交绘制由FrameGraph中的ImGuiPass负责执行 */
		ImGui::Render();
	}

	void ImGuiLayer::RenderPlatformWindows()
	{
		PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::AddFrameGraphPass(FrameGraph& frame_graph)
	{
		PROFILE_FUNCTION();

		auto* imgui_renderer = m_Renderer.get();
		if (!imgui_renderer)
			return;

		auto& window = Application::Instance()->GetWindow();
		const uint32_t target_width = window.GetWidth();
		const uint32_t target_height = window.GetHeight();

		frame_graph.AddPass<ImGuiPassData>("ImGuiPass",
			/* Setup */
			[](FrameGraphBuilder& builder, ImGuiPassData&)
			{
				/* ImGuiPass不通过FrameGraph管理其Attachments（它直接渲染到主窗口默认RT），
				 * 因此标记为SideEffect，避免被Cull。 */
				builder.AsSideEffect(true);
			},
			/* Execute */
			[imgui_renderer, target_width, target_height](const FrameGraphResources&, const ImGuiPassData&)
			{
				ImDrawData* draw_data = ImGui::GetDrawData();
				if (!draw_data)
					return;

				auto render_api = Renderer::GetRenderAPI();
				if (!render_api)
					return;

#ifdef PLATFORM_MACOS
				/* Metal：直接绑定主窗口drawable作为ColorAttachment，LoadActionLoad保留前序Pass的结果，
				 * 并复用当前CommandBuffer（不在ImGui内部创建/提交CommandBuffer）。 */
				auto metal_render_api = std::dynamic_pointer_cast<MetalRenderAPI>(render_api);
				if (metal_render_api)
				{
					auto* drawable = metal_render_api->GetCurrentDrawable();
					if (!drawable)
					{
						CORE_LOG_ERROR("ImGuiPass: no valid drawable available");
						return;
					}

					/* 若仍有上一Pass未结束的encoder，先结束 */
					if (metal_render_api->GetCurrentRenderEncoder())
						metal_render_api->EndRenderPass();

					/* 构造ImGuiPass专用的RenderPassDescriptor（不清除drawable内容） */
					MTL::RenderPassDescriptor* pass_desc = MTL::RenderPassDescriptor::alloc()->init();
					auto* color_attachment = pass_desc->colorAttachments()->object(0);
					color_attachment->setTexture(drawable->texture());
					color_attachment->setLoadAction(MTL::LoadActionLoad);
					color_attachment->setStoreAction(MTL::StoreActionStore);

					/* 使用统一的BeginRenderPass以复用当前CommandBuffer */
					metal_render_api->BeginRenderPass(pass_desc);

					/* 设置视口为整个主窗口 */
					render_api->SetViewport(0, 0, target_width, target_height);

					/* 提交ImGui绘制 */
					imgui_renderer->RenderDrawData(draw_data);

					metal_render_api->EndRenderPass();

					pass_desc->release();
					return;
				}
#endif
				/* OpenGL/其他：绑定默认FBO，ImGuiRenderer会处理状态、投影、裁剪等 */
				render_api->SetViewport(0, 0, target_width, target_height);
				imgui_renderer->RenderDrawData(draw_data);
			}
		);
	}

	void ImGuiLayer::SetDefaultStyle()
	{
		//ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text]					= ImVec4(1.00f, 1.00f, 1.00f, 1.00f); /* 文字默认颜色 */
		colors[ImGuiCol_TextDisabled]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f); /* 禁用的控件文字颜色 */
		colors[ImGuiCol_WindowBg]				= ImVec4(0.12f, 0.12f, 0.12f, 1.00f); /* 窗口背景颜色 */
		colors[ImGuiCol_ChildBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg]				= ImVec4(0.12f, 0.12f, 0.12f, 0.94f); /* 弹窗背景颜色 */
		colors[ImGuiCol_Border]					= ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow]			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg]				= ImVec4(0.08f, 0.08f, 0.08f, 0.74f);
		colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.43f, 0.40f, 0.35f, 0.40f);
		colors[ImGuiCol_FrameBgActive]			= ImVec4(0.43f, 0.40f, 0.35f, 0.67f);
		colors[ImGuiCol_TitleBg]				= ImVec4(0.18f, 0.18f, 0.18f, 1.00f); /* 窗口标题背景颜色 */
		colors[ImGuiCol_TitleBgActive]			= ImVec4(0.20f, 0.20f, 0.20f, 1.00f); /* 活动窗口标题背景颜色 */
		colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.18f, 0.18f, 0.18f, 0.50f); /*  */
		colors[ImGuiCol_MenuBarBg]				= ImVec4(0.14f, 0.14f, 0.14f, 1.00f); /* 菜单栏背景颜色 */
		colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark]				= ImVec4(0.43f, 0.40f, 0.35f, 1.00f);
		colors[ImGuiCol_SliderGrab]				= ImVec4(0.40f, 0.38f, 0.32f, 1.00f);
		colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.43f, 0.40f, 0.35f, 1.00f);
		colors[ImGuiCol_Button]					= ImVec4(0.43f, 0.40f, 0.35f, 0.40f);
		colors[ImGuiCol_ButtonHovered]			= ImVec4(0.43f, 0.40f, 0.35f, 1.00f);
		colors[ImGuiCol_ButtonActive]			= ImVec4(0.56f, 0.53f, 0.35f, 1.00f);
		colors[ImGuiCol_Header]					= ImVec4(0.43f, 0.40f, 0.35f, 0.31f); /* 窗口标题背景颜色 */
		colors[ImGuiCol_HeaderHovered]			= ImVec4(0.43f, 0.40f, 0.35f, 0.80f); /* 鼠标悬停窗口标题背景颜色 */
		colors[ImGuiCol_HeaderActive]			= ImVec4(0.43f, 0.40f, 0.35f, 1.00f); /* 活动窗口标题背景颜色 */
		colors[ImGuiCol_Separator]				= colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive]		= ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip]				= ImVec4(0.43f, 0.40f, 0.35f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.43f, 0.40f, 0.35f, 0.67f);
		colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.43f, 0.40f, 0.35f, 0.95f);
		colors[ImGuiCol_Tab]					= ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered]				= colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive]				= ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused]			= ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive]		= ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f); /* 活动窗口标签颜色 */
		colors[ImGuiCol_DockingPreview]			= colors[ImGuiCol_HeaderActive]/* * ImVec4(1.0f, 1.0f, 1.0f, 0.7f)*/;
		colors[ImGuiCol_DockingEmptyBg]			= ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines]				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram]			= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg]			= ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong]		= ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight]		= ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt]			= ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.43f, 0.40f, 0.35f, 0.35f);
		colors[ImGuiCol_DragDropTarget]			= ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight]			= ImVec4(0.43f, 0.40f, 0.35f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
	}
}
