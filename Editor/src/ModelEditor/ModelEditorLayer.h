#pragma once
#include "EditorBuiltinCamera.h"
#include "EditorResourceBrowser.h"
#include "ModelInfo.h"

namespace Wuya
{
	/* ģ�ͱ༭��
	 * ��ģ�͵���
	 * �����ʱ༭
	 */
	class ModelEditorLayer final : public ILayer
	{
	public:
		ModelEditorLayer();
		~ModelEditorLayer() override = default;

		void OnAttached() override;
		void OnDetached() override;
		void OnUpdate(float delta_time) override;
		void OnImGuiRender() override;
		void OnEvent(IEvent* event) override;

		/* ����� */
		void Active(bool active = true) { m_IsActivated = active; }
		bool IsActivated() const { return m_IsActivated; }

		/* ����ģ�� */
		void ImportModel();
		/* ����ģ��(.mesh & .mtl) */
		void ExportMeshAndMtl();
		/* ����Mesh */
		void ExportMesh(const std::string& path);

	private:
		/* ��ʾ�������ӿ� */
		void ShowSceneViewportUI();
		/* ��ʾģ�ͱ༭UI */
		void ShowModelParamsUI();
		/* ��Ӧ��ק�ļ��������� */
		void OnDragItemToScene(const std::filesystem::path& path);

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

		/* ��ǰ�����Ƿ񱻼��� */
		bool m_IsActivated{ false };
	};
}
