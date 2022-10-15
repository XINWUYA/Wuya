#pragma once
#include "EditorBuiltinCamera.h"

namespace Wuya
{
	/* ģ�ͱ༭��
	 * ��ģ�͵���
	 * �����ʱ༭
	 */
	class ModelEditorLayer : public ILayer
	{
	public:
		ModelEditorLayer();
		~ModelEditorLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

	private:
		/* ��ʾ�˵���UI */
		void ShowMenuUI();
		/* ��ʾ�������ӿ� */
		void ShowSceneViewportUI();

		/* ����ģ�� */
		void ImportModel();
		/* ����ģ�� */
		void ExportModel();

		/* �༭����� */
		UniquePtr<EditorCamera> m_pEditorCamera{ nullptr };

		/* Ĭ�ϳ��� */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* ��ǰģ������·�� */
		std::string m_ModelPath{};
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};
	};
}
