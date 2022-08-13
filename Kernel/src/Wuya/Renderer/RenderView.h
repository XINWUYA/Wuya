#pragma once
#include "RenderCommon.h"
#include <glm/glm.hpp>

#include "FrameGraph/FrameGraphResourceHandle.h"

namespace Wuya
{
	class Camera;
	class Scene;
	class MeshSegment;
	class Texture;
	class FrameGraph;

	/* 对当前RenderView可见的模型 */
	struct VisibleMeshObject
	{
		glm::mat4 Local2WorldMat;
		SharedPtr<MeshSegment> MeshSegment;

		VisibleMeshObject() = default;
		VisibleMeshObject(const glm::mat4& local_2_world, const SharedPtr<class MeshSegment>& mesh_segment)
			: Local2WorldMat(local_2_world), MeshSegment(mesh_segment)
		{}
	};

	/* 视图类：
	 * 每个相机当对应一个RenderView
	 * 每个RenderView单独对应一个FrameGraph
	 * 每个RenderView单独做裁剪
	 * 每个RenderView单独对应一套后处理
	 */
	class RenderView
	{
	public:
		RenderView(const std::string& name);
		~RenderView();

		/* 设置名称 */
		void SetName(const std::string& name) { m_DebugName = name; }
		const std::string& GetName() const { return m_DebugName; }

		/* 设置裁剪相机 */
		void SetCullingCamera(const SharedPtr<Camera>& camera) { m_pCullingCamera = camera; }
		const SharedPtr<Camera>& GetCullingCamera() const { return m_pCullingCamera; }

		/* 设置启用视锥体剔除 */
		void SetEnableFrustumCulling(bool enable) { m_IsEnableCulling = enable; }
		bool IsEnableFrustumCulling() const { return m_IsEnableCulling; }

		/* 设置视口区域 */
		void SetViewportRegion(const ViewportRegion& region);
		const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* 设置所属的Scene */
		void SetOwnerScene(const SharedPtr<Scene>& scene) { m_pOwnerScene = scene; }
		const SharedPtr<Scene>& GetOwnerScene() const { return m_pOwnerScene; }

		/* 设置RenderTarget */
		void SetRenderTargetHandle(FrameGraphResourceHandle handle) { m_RenderTargetHandle = handle; }
		SharedPtr<Texture> GetRenderTarget() const;

		/* 获取FrameGraph */
		const SharedPtr<FrameGraph>& GetFrameGraph() const { return m_pFrameGraph; }

		/* 准备一帧的RenderView数据 */
		void Prepare();

		/* 执行渲染当前View */
		void Execute();

		/* 获取可见MeshSegment */
		const std::vector<VisibleMeshObject>& GetVisibleMeshObjects() const { return m_VisibleMeshObjects; }

	private:
		/* 视锥体剔除，仅保留对当前RenderView可见的对象 */
		void PrepareVisibleObjects();

		/* RenderView名 */
		std::string m_DebugName{ "Unnamed RenderView" };
		/* 视口区域 */
		ViewportRegion m_ViewportRegion{};
		/* 是否启用视锥体剔除 */
		bool m_IsEnableCulling{ true };
		/* 相机 */
		SharedPtr<Camera> m_pCullingCamera{ nullptr };
		/* 所属的Scene */
		SharedPtr<Scene> m_pOwnerScene{ nullptr };
		/* 视锥体剔除之后，对当前可见MeshSegment */
		std::vector<VisibleMeshObject> m_VisibleMeshObjects{};
		/* 当前View的渲染结果输出到该RenderTarget */
		FrameGraphResourceHandle m_RenderTargetHandle{};
		/* FrameGraph */
		SharedPtr<FrameGraph> m_pFrameGraph{ nullptr };
	};
}
