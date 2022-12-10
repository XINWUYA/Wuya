#pragma once
#include "EditorBuiltinCamera.h"
#include "ModelInfo.h"

namespace Wuya
{
	/* 模型编辑器
	 * ：模型导入
	 * ：材质编辑
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
		/* 显示菜单栏UI */
		void ShowMenuUI();
		/* 显示主场景视口 */
		void ShowSceneViewportUI();
		/* 显示模型编辑UI */
		void ShowModelParamsUI();

		/* 导入模型 */
		void ImportModel();
		/* 导出模型(.mesh & .mtl) */
		void ExportMeshAndMtl();

		/* 更新模型 */
		void UpdateModel();
		/* 更新材质：当用户修改时根据材质参数设置材质 */
		void UpdateMaterial(const SharedPtr<Material>& material, const MaterialParams& material_params);

		/* 默认场景 */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* 编辑器相机 */
		UniquePtr<EditorCamera> m_pEditorCamera{ nullptr };
		/* 视口范围: x: width_min; y: height_min; z: width_max; w: height_max */
		ViewportRegion m_ViewportRegion{};

		/* 从文件中加载模型信息 */
		UniquePtr<ModelInfo> m_pModelInfo{ nullptr };

		/* 当前模型 */
		SharedPtr<Model> m_pModel{ nullptr };
		/* 当前模型对应的材质组，其中每个Material对应一个子模型 */
		SharedPtr<MaterialGroup> m_pMaterialGroup{ nullptr };
	};
}
