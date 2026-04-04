#include "Pch.h"
#include "ImGuiLayer.h"
#include "ImGuiRenderer.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include "Wuya/Application/Application.h"
#include "Wuya/Renderer/Renderer.h"
#include "Wuya/Scene/SceneCommon.h"

namespace Wuya
{
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
		std::string ini_path = std::string(ASSETS_PATH) + "/../imgui.ini";
		io.IniFilename = strdup(ini_path.c_str());  // Note: memory leak is acceptable here for small string

		const float fontSize = 18.0f;
		io.Fonts->AddFontFromFileTTF(ABSOLUTE_PATH("EditorRes/fonts/msyh.ttf").c_str(), fontSize);

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

		Renderer::GetRenderAPI()->PushDebugGroup("ImGuiPass");

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

	void ImGuiLayer::End()
	{
		PROFILE_FUNCTION();

		ImGui::Render();
		
		// Use our custom renderer to draw
		m_Renderer->RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		Renderer::GetRenderAPI()->PopDebugGroup();
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
