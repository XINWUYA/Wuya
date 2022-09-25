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

	/* ��ͼ�ࣺ
	 * ÿ���������Ӧһ��RenderView
	 * ÿ��RenderView������Ӧһ��FrameGraph
	 * ÿ��RenderView�������ü�
	 * ÿ��RenderView������Ӧһ�׺���
	 */
	class RenderView
	{
	public:
		RenderView(std::string name);
		~RenderView();

		/* �������� */
		void SetName(const std::string& name) { m_DebugName = name; }
		const std::string& GetName() const { return m_DebugName; }

		/* ���òü���� */
		void SetCullingCamera(const SharedPtr<Camera>& camera) { m_pCullingCamera = camera; }
		const SharedPtr<Camera>& GetCullingCamera() const { return m_pCullingCamera.lock(); }

		/* ����������׶���޳� */
		void SetEnableFrustumCulling(bool enable) { m_IsEnableCulling = enable; }
		bool IsEnableFrustumCulling() const { return m_IsEnableCulling; }

		/* �����ӿ����� */
		void SetViewportRegion(const ViewportRegion& region);
		const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* ����������Scene */
		void SetOwnerScene(const SharedPtr<Scene>& scene) { m_pOwnerScene = scene; }

		/* ����RenderTarget */
		void SetRenderTargetHandle(FrameGraphResourceHandle handle) { m_RenderTargetHandle = handle; }
		SharedPtr<Texture> GetRenderTarget() const;

		/* ��ȡFrameGraph */
		const SharedPtr<FrameGraph>& GetFrameGraph() const { return m_pFrameGraph; }

		/* �洢��Pass��FrameBuffer */
		void EmplacePassFrameBuffer(const std::string& name, const SharedPtr<FrameBuffer>& frame_buffer);
		const SharedPtr<FrameBuffer>& GetPassFrameBuffer(const std::string& name) const;

		/* ׼��һ֡��RenderView���� */
		void Prepare();

		/* ִ����Ⱦ��ǰView */
		void Execute();

		/* ��ȡ�ɼ�MeshSegment */
		const std::vector<VisibleMeshObject>& GetVisibleMeshObjects() const { return m_VisibleMeshObjects; }

	private:
		/* ��׶���޳����������Ե�ǰRenderView�ɼ��Ķ��� */
		void PrepareVisibleObjects();

		/* RenderView�� */
		std::string m_DebugName{ "Unnamed RenderView" };
		/* �ӿ����� */
		ViewportRegion m_ViewportRegion{};
		/* �Ƿ�������׶���޳� */
		bool m_IsEnableCulling{ true };
		/* ��� */
		WeakPtr<Camera> m_pCullingCamera;
		/* ������Scene */
		WeakPtr<Scene> m_pOwnerScene;
		/* ��׶���޳�֮�󣬶Ե�ǰ�ɼ�MeshSegment */
		std::vector<VisibleMeshObject> m_VisibleMeshObjects{};
		/* ��ǰView����Ⱦ����������RenderTarget */
		FrameGraphResourceHandle m_RenderTargetHandle{};
		/* FrameGraph */
		SharedPtr<FrameGraph> m_pFrameGraph{ nullptr };
		/* �ռ���ǰRenderView��Ӧ��FrameGraph�и�Pass�׶ε�FrameBuffer������ReadPixels<PassName, FrameBufferPtr> */
		std::unordered_map<std::string, SharedPtr<FrameBuffer>> m_PassFrameBuffers{};
	};
}
