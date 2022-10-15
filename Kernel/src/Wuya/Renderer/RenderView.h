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

	/* �Ե�ǰRenderView�ɼ���ģ�� */
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

	/* Ӱ�쳡������Ч��Դ */
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

	/* ��ͼ�ࣺ
	 * ÿ���������Ӧһ��RenderView
	 * ÿ��RenderView������Ӧһ��FrameGraph
	 * ÿ��RenderView�������ü�
	 * ÿ��RenderView������Ӧһ�׺���
	 */
	class RenderView
	{
	public:
		RenderView(std::string name, Camera* owner_camera);
		~RenderView();

		/* �������� */
		void SetDebugName(const std::string& name) { m_DebugName = name; }
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

		/* ��ȡ�ü���� */
		[[nodiscard]] const Camera* GetCullingCamera() const { return m_pOwnerCamera; }

		/* ����������׶���޳� */
		void SetEnableFrustumCulling(bool enable) { m_IsEnableCulling = enable; }
		[[nodiscard]] bool IsEnableFrustumCulling() const { return m_IsEnableCulling; }

		/* �����ӿ����� */
		void SetViewportRegion(const ViewportRegion& region);
		[[nodiscard]] const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* ����������Scene */
		void SetOwnerScene(const SharedPtr<Scene>& scene) { m_pOwnerScene = scene; }

		/* ����RenderTarget */
		void SetRenderTargetHandle(FrameGraphResourceHandle handle) { m_RenderTargetHandle = handle; }
		[[nodiscard]] SharedPtr<Texture> GetRenderTarget() const;

		/* ��ȡFrameGraph */
		[[nodiscard]] const SharedPtr<FrameGraph>& GetFrameGraph() const { return m_pFrameGraph; }

		/* �洢��Pass��FrameBuffer */
		void EmplacePassFrameBuffer(const std::string& name, const SharedPtr<FrameBuffer>& frame_buffer);
		[[nodiscard]] const SharedPtr<FrameBuffer>& GetPassFrameBuffer(const std::string& name) const;

		/* ׼��һ֡��RenderView���� */
		void Prepare();

		/* ִ����Ⱦ��ǰView */
		void Execute();

		/* ��ȡ�ɼ�MeshSegment */
		[[nodiscard]] const std::vector<VisibleMeshObject>& GetVisibleMeshObjects() const { return m_VisibleMeshObjects; }

		/* ��ȡ��Դ */
		[[nodiscard]] const std::vector<ValidLight>& GetValidLights() const { return m_ValidLights; }

	private:
		/* ��׶���޳����������Ե�ǰRenderView�ɼ��Ķ��� */
		void PrepareVisibleObjects();
		/* ׼����Դ��Ϣ */
		void PrepareLights();

		/* RenderView�� */
		std::string m_DebugName{ "Unnamed RenderView" };
		/* �ӿ����� */
		ViewportRegion m_ViewportRegion{};
		/* �Ƿ�������׶���޳� */
		bool m_IsEnableCulling{ true };
		/* ��� */
		Camera* m_pOwnerCamera{ nullptr };
		/* ������Scene */
		WeakPtr<Scene> m_pOwnerScene;
		/* ��׶���޳�֮�󣬶Ե�ǰ�ɼ�MeshSegment */
		std::vector<VisibleMeshObject> m_VisibleMeshObjects{};
		/* �Գ�������Ӱ��Ĺ�Դ */
		std::vector<ValidLight> m_ValidLights{};
		/* ��ǰView����Ⱦ����������RenderTarget */
		FrameGraphResourceHandle m_RenderTargetHandle{};
		/* FrameGraph */
		SharedPtr<FrameGraph> m_pFrameGraph{ nullptr };
		/* �ռ���ǰRenderView��Ӧ��FrameGraph�и�Pass�׶ε�FrameBuffer������ReadPixels<PassName, FrameBufferPtr> */
		std::unordered_map<std::string, SharedPtr<FrameBuffer>> m_PassFrameBuffers{};
	};
}
