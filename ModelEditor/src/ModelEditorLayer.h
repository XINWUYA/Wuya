#pragma once
#include "EditorBuiltinCamera.h"
#include "ModelInfo.h"

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
		/* ��ʾģ�ͱ༭UI */
		void ShowModelParamsUI();

		/* ����ģ�� */
		void ImportModel();
		/* ����ģ��(.mesh & .mtl) */
		void ExportMeshAndMtl();

		/* ����ģ�� */
		void UpdateModel();
		/* ���²��ʣ����ݲ��ʲ������ò��� */
		void UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params);

		/* �༭����� */
		UniquePtr<EditorCamera> m_pEditorCamera{ nullptr };
		/* ��ǰģ����Ϣ */
		UniquePtr<ModelInfo> m_pModelInfo{ nullptr };
		/* ��ǰ������Ϣ������ÿ����ģ�Ͷ�Ӧһ��Material */
		std::vector<SharedPtr<Material>> m_Materials{};
		/* Ĭ�ϳ��� */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};
	};
}
