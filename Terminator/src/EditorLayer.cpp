#include "EditorLayer.h"
#include <imgui.h>

EditorLayer::EditorLayer()
	: ILayer("EditorLayer")
{
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("Stats");
	ImGui::End();
}
