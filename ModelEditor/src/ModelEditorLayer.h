#pragma once
#include <Kernel.h>

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

		/* 导入模型 */
		void ImportModel();
		/* 导出模型 */
		void ExportModel();

		/* 默认场景 */
		SharedPtr<Scene> m_pDefaultScene{ nullptr };
		/* 当前模型所在路径 */
		std::string m_ModelPath{};

	};
}
