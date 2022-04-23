#pragma once
#include <Wuya/Core/Layer.h>
#include <Wuya.h>
#include "EditorSceneHierarchy.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	class KeyPressedEvent;
	class MouseButtonPressedEvent;
}

class EditorLayer final : public Wuya::ILayer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	void OnAttached() override;
	void OnDetached() override;
	void OnUpdate(float delta_time) override;
	void OnImGuiRender() override;
	void OnEvent(Wuya::IEvent* event) override;

private:
	/* 更新视口 */
	void UpdateViewport();
	/* 响应键盘 */
	bool OnKeyPressed(Wuya::KeyPressedEvent* event);
	/* 响应鼠标 */
	bool OnMouseButtonPressed(Wuya::MouseButtonPressedEvent* event);
	/* 切换运行模式 */
	void OnPlayModeChanged();

	/* 新建场景 */
	void NewScene();
	/* 打开场景：通过弹窗找到指定场景文件并打开 */
	void OpenScene();
	/* 保存场景 */
	void SaveScene();
	/* 打开指定场景 */
	void OpenScene(const std::filesystem::path& path);

	/* 显示菜单栏UI */
	void ShowMenuUI();
	/* 显示场景控制UI */
	void ShowSceneControllerUI();
	/* 显示主场景视口 */
	void ShowSceneViewportUI();
	/* 显示渲染统计信息 */
	void ShowStatisticInfoUI();

	/* 执行模式 */
	enum class PlayMode : uint8_t
	{
		Edit = 0,	/* 编辑模式 */
		Runtime		/* 运行模式 */
	};

	Wuya::SharedPtr<Wuya::VertexArray> m_pVertexArray;
	Wuya::SharedPtr<Wuya::Texture2D> m_pTexture2D;
	Wuya::UniquePtr<Wuya::ShaderLibrary> m_pShaderLibrary;
	Wuya::SharedPtr<Wuya::EditorCamera> m_pEditorCamera;
	Wuya::SharedPtr<Wuya::OrthographicCameraController> m_pOrthographicCameraController;
	Wuya::SharedPtr<Wuya::FrameBuffer> m_pFrameBuffer;

	/* 主场景 */
	Wuya::SharedPtr<Wuya::Scene> m_pMainScene;
	/* 场景实体管理窗口 */
	EditorSceneHierarchy m_SceneHierarchy;
	/* 资源管理窗口 */
	EditorResourceBrowser m_ResourceBrowser;
	/* */
	Wuya::Entity m_HoveredEntity;
	/* 视口尺寸 */
	glm::uvec2 m_ViewportSize{};
	/* 视口窗口被激活 */
	bool m_IsViewportFocused{ false };
	/* 鼠标停留在视口上 */
	bool m_IsViewportHovered{ false };
	/* 默认为编辑模式 */
	PlayMode m_PlayMode{ PlayMode::Edit };
	/* 移动，旋转，缩放UI */
	int m_GizmoType = -1;
};

