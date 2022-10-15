#pragma once
#include <Kernel.h>

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

		/* ����ģ�� */
		void ImportModel();
		/* ����ģ�� */
		void ExportModel();

		/* Ĭ�ϳ��� */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* ��ǰģ������·�� */
		std::string m_ModelPath{};

	};
}
