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
		/* ���²��ʣ����û��޸�ʱ���ݲ��ʲ������ò��� */
		void UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params);

		/* Ĭ�ϳ��� */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* �༭����� */
		UniquePtr<EditorCamera> m_pEditorCamera{ nullptr };
		/* �ӿڷ�Χ: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};

		/* ���ļ��м���ģ����Ϣ */
		UniquePtr<ModelInfo> m_pModelInfo{ nullptr };

		/* ��ǰģ�� */
		SharedPtr<Model> m_pModel{ nullptr };
		/* ��ǰģ�Ͷ�Ӧ�Ĳ����飬����ÿ��Material��Ӧһ����ģ�� */
		SharedPtr<MaterialGroup> m_pMaterialGroup{ nullptr };
	};
}
