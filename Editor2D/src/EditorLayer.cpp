#include "EditorLayer.h"
#include <imgui.h>

EditorLayer::EditorLayer()
	: ILayer("EditorLayer")
{
}

void EditorLayer::OnAttached()
{
	// Texture
	m_pTexture2D = Wuya::Texture2D::Create("assets/textures/checker-board.png");


	// Camera Controller
	m_pCameraController = Wuya::CreateUniquePtr<Wuya::OrthographicCameraController>(1280.0f / 720.0f, true);

	Wuya::Renderer2D::Init();
}

void EditorLayer::OnDetached()
{
	ILayer::OnDetached();
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	auto viewport_size = { viewport_panel_size.x, viewport_panel_size.y };

	ImGui::End();
}

void EditorLayer::OnEvent(Wuya::IEvent* event)
{
	m_pCameraController->OnEvent(event);
}

void EditorLayer::OnUpdate(float delta_time)
{
	m_pCameraController->OnUpdate(delta_time);

	Wuya::Renderer::SetClearColor(glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	Wuya::Renderer::Clear();

	Wuya::Renderer2D::BeginScene(m_pCameraController->GetCamera());
	Wuya::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });

	for (float y = -5.0f; y < 5.0f; y += 1.5f)
	{
		for (float x = -5.0f; x < 5.0f; x += 1.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			Wuya::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
		}
	}
	Wuya::Renderer2D::EndScene();
}
