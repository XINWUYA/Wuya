#pragma once
#include <Kernel.h>
#include "EditorSceneHierarchy.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	/* Editor层类 */
	class EditorLayer final : public ILayer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

	private:
		/* 更新视口 */
		void UpdateViewport();
		/* 响应键盘 */
		bool OnKeyPressed(class KeyPressedEvent* event);
		/* 响应鼠标 */
		bool OnMouseButtonPressed(class MouseButtonPressedEvent* event);
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
		/* 选中Entity时显示操作Gizmo */
		void ShowOperationGizmoUI();

		/* 鼠标选中Entity时的响应 */
		void CheckMouseSelectEntity();

		/* 执行模式 */
		enum class PlayMode : uint8_t
		{
			Edit = 0,	/* 编辑模式 */
			Runtime		/* 运行模式 */
		};

		UniquePtr<ShaderLibrary> m_pShaderLibrary;
		SharedPtr<class EditorCamera> m_pEditorCamera;
		SharedPtr<OrthographicCameraController> m_pOrthographicCameraController;

		/* 主场景 */
		SharedPtr<Scene> m_pMainScene;
		/* 场景实体管理窗口 */
		EditorSceneHierarchy m_SceneHierarchy;
		/* 资源管理窗口 */
		EditorResourceBrowser m_ResourceBrowser;
		/* 选中实体 */
		Entity m_HoveredEntity;
		/* 视口范围: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};
		/* 视口窗口被激活 */
		bool m_IsViewportFocused{ false };
		/* 鼠标停留在视口上 */
		bool m_IsViewportHovered{ false };
		/* 默认为编辑模式 */
		PlayMode m_PlayMode{ PlayMode::Edit };
		/* 移动，旋转，缩放UI */
		int m_GizmoType = -1;
	};
}
