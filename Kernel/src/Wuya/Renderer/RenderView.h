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
	class FrameBuffer;
	class FrameGraph;
	class ShadowMapManager;

	/* 对当前RenderView可见的模型 */
	struct VisibleMeshObject
	{
		int ObjectId{ -1 };
		glm::mat4 Local2WorldMat;
		SharedPtr<MeshSegment> MeshSegment;

		VisibleMeshObject() = default;
		VisibleMeshObject(int object_id, const glm::mat4& local_2_world, const SharedPtr<class MeshSegment>& mesh_segment)
			: ObjectId(object_id), Local2WorldMat(local_2_world), MeshSegment(mesh_segment)
		{}
	};

	/* 影响场景的有效光源 */
	struct ValidLight
	{
		uint32_t LightType{ 0 };
		glm::vec4 ColorIntensity{ 1.0f, 1.0f, 1.0f, 1.0f }; /* rgb: color, a: intensity */
		glm::vec3 LightDir{ 0.0f };
		glm::vec3 LightPos{ 0.0f };
		bool IsCastShadow{ false };

		ValidLight() = default;
		ValidLight(uint32_t type, const glm::vec4& color_intensity, const glm::vec3& dir, const glm::vec3& pos, bool cast_shadow)
			: LightType(type), ColorIntensity(color_intensity), LightDir(dir), LightPos(pos), IsCastShadow(cast_shadow)
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
		RenderView(std::string name, Camera* owner_camera);
		~RenderView();

		/* 设置名称 */
		void SetDebugName(const std::string& name) { m_DebugName = name; }
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* 获取裁剪相机 */
		[[nodiscard]] const Camera* GetCullingCamera() const { return m_pOwnerCamera; }

		/* 设置启用视锥体剔除 */
		void SetEnableFrustumCulling(bool enable) { m_IsEnableCulling = enable; }
		[[nodiscard]] bool IsEnableFrustumCulling() const { return m_IsEnableCulling; }

		/* 设置视口区域 */
		void SetViewportRegion(const ViewportRegion& region);
		[[nodiscard]] const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* 设置所属的Scene */
		void SetOwnerScene(const SharedPtr<Scene>& scene) { m_pOwnerScene = scene; }

		/* 设置RenderTarget */
		void SetRenderTargetHandle(FrameGraphResourceHandle handle) { m_RenderTargetHandle = handle; }
		[[nodiscard]] SharedPtr<Texture> GetRenderTarget() const;

		/* 获取FrameGraph */
		[[nodiscard]] const SharedPtr<FrameGraph>& GetFrameGraph() const { return m_pFrameGraph; }

		/* 存储各Pass的FrameBuffer */
		void EmplacePassFrameBuffer(const std::string& name, const SharedPtr<FrameBuffer>& frame_buffer);
		[[nodiscard]] const SharedPtr<FrameBuffer>& GetPassFrameBuffer(const std::string& name) const;

		/* 准备一帧的RenderView数据 */
		void Prepare();

		/* 执行渲染当前View */
		void Execute();

		/* 获取可见MeshSegment */
		[[nodiscard]] const std::vector<VisibleMeshObject>& GetVisibleMeshObjects() const { return m_VisibleMeshObjects; }

		/* 获取光源 */
		[[nodiscard]] const std::vector<ValidLight>& GetValidLights() const { return m_ValidLights; }

	private:
		/* 视锥体剔除，仅保留对当前RenderView可见的对象 */
		void PrepareVisibleObjects();
		/* 准备光源信息 */
		void PrepareLights();

		/* RenderView名 */
		std::string m_DebugName{ "Unnamed RenderView" };
		/* 视口区域 */
		ViewportRegion m_ViewportRegion{};
		/* 是否启用视锥体剔除 */
		bool m_IsEnableCulling{ true };
		/* 相机 */
		Camera* m_pOwnerCamera{ nullptr };
		/* 所属的Scene */
		WeakPtr<Scene> m_pOwnerScene;
		/* 视锥体剔除之后，对当前可见MeshSegment */
		std::vector<VisibleMeshObject> m_VisibleMeshObjects{};
		/* 对场景产生影响的光源 */
		std::vector<ValidLight> m_ValidLights{};
		/* 当前View的渲染结果输出到该RenderTarget */
		FrameGraphResourceHandle m_RenderTargetHandle{};
		/* FrameGraph */
		SharedPtr<FrameGraph> m_pFrameGraph{ nullptr };
		/* 是否使用自定义的FrameGraph覆盖 */
		bool m_IsOverrideFrameGraph{ false };
		/* 收集当前RenderView对应的FrameGraph中各Pass阶段的FrameBuffer，用于ReadPixels<PassName, FrameBufferPtr> */
		std::unordered_map<std::string, SharedPtr<FrameBuffer>> m_PassFrameBuffers{};

		/* 是否开启投影 */
		bool m_IsHasShadowCast{ false };
		/* 阴影管理 */
		SharedPtr<ShadowMapManager> m_pShadowMapManager{ nullptr };
	};
}
