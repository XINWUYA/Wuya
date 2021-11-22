#include "pch.h"
#include "ImguiLayer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <Wuya/Application/Application.h>

namespace Wuya
{
	ImguiLayer::ImguiLayer()
		: ILayer("ImguiLayer")
	{
	}

	void ImguiLayer::OnAttached()
	{
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		//float fontSize = 18.0f;// *2.0f;
		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		SetDarkThemeColors();

		// Setup Platform/Renderer bindings
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Instance()->GetWindow().GetNativeWindow());
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

	void ImguiLayer::OnDetached()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImguiLayer::OnUpdate(float delta_time)
	{
		Begin();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = delta_time;
		io.DisplaySize = ImVec2((float)Application::Instance()->GetWindow().GetWidth(), (float)Application::Instance()->GetWindow().GetHeight()); 

		bool show = true;
		ImGui::ShowDemoWindow(&show);

		End();
	}

	void ImguiLayer::OnEvent(IEvent* event)
	{
		if (m_IsBlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event->Handled |= event->IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event->Handled |= event->IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImguiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImguiLayer::End()
	{
		/*ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Application::Instance()->GetWindow().GetWidth(), (float)Application::Instance()->GetWindow().GetHeight());*/

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImguiLayer::SetDarkThemeColors()
	{
	}
}