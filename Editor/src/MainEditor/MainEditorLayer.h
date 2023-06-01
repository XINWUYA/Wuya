#pragma once
#include "EditorCommon.h"
#include "EditorResourceBrowser.h"

namespace Wuya
{
	/* Editor层类 */
	class MainEditorLayer final : public ILayer
	{
	public:
		MainEditorLayer();
		~MainEditorLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

	private:
		/* 响应键盘 */
		bool OnKeyPressed(class KeyPressedEvent* event);
		/* 响应鼠标 */
		bool OnMouseButtonPressed(class MouseButtonPressedEvent* event);

		/* 新建场景 */
		void NewScene();
		/* 导入场景：通过弹窗找到指定场景文件并打开 */
		void ImportScene();
		/* 保存场景 */
		void SaveScene();
		/* 保存场景到指定路径 */
		void SaveSceneAs();

		/* 显示菜单栏UI */
		void ShowMenuUI();
		/* 显示场景控制UI */
		void ShowSceneControllerUI();
		/* 显示渲染统计信息 */
		void ShowStatisticInfoUI();

		/* 通知其他主窗口 */
		void NotifyGizmoTypeChanged();
		void NotifyPlayModeChanged();

		/* 资源管理窗口 */
		EditorResourceBrowser m_ResourceBrowser;
		/* 默认为编辑模式 */
		PlayMode m_PlayMode{ PlayMode::Edit };
		/* 移动，旋转，缩放UI */
		int m_GizmoType = -1;

		/* 窗口开关 */
		bool m_ActiveSceneEditor{ false };
		bool m_ActiveModelEditor{ false };
	};
}
