#pragma once
#include "RenderCommon.h"
#include <glm/glm.hpp>

namespace Wuya
{
	class Camera;
	class Scene;
	class MeshSegment;
	class Texture;

	struct VisibleMeshObject
	{
		glm::mat4 Local2WorldMat;
		SharedPtr<MeshSegment> MeshSegment;

		VisibleMeshObject() = default;
		VisibleMeshObject(const glm::mat4& local_2_world, const SharedPtr<class MeshSegment> mesh_segment)
			: Local2WorldMat(local_2_world), MeshSegment(mesh_segment)
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
		RenderView() = default;
		RenderView(const std::string& name, const SharedPtr<Camera>& culling_camera, bool enable_culling = true);
		~RenderView();

		/* �������� */
		void SetName(const std::string& name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }

		/* ���òü���� */
		void SetCullingCamera(const SharedPtr<Camera>& camera) { m_pCullingCamera = camera; }
		const SharedPtr<Camera>& GetCullingCamera() const { return m_pCullingCamera; }

		/* ����������׶���޳� */
		void SetEnableFrustumCulling(bool enable) { m_IsEnableCulling = enable; }
		bool IsEnableFrustumCulling() const { return m_IsEnableCulling; }

		/* �����ӿ����� */
		void SetViewportRegion(const ViewportRegion& region);
		const ViewportRegion& GetViewportRegion() const { return m_ViewportRegion; }

		/* ����������Scene */
		void SetOwnerScene(const SharedPtr<Scene>& scene) { m_pOwnerScene = scene; }
		const SharedPtr<Scene>& GetOwnerScene() const { return m_pOwnerScene; }

		/* ����RenderTarget Texture */
		void SetRenderTargetTexture(const SharedPtr<Texture>& texture) { m_pRenderTargetTexture = texture; }
		const SharedPtr<Texture>& GetRenderTargetTexture() const { return m_pRenderTargetTexture; }

		/* ׼��һ֡��RenderView���� */
		void Prepare();

		/* ��ȡ�ɼ�MeshSegment */
		const std::vector<VisibleMeshObject>& GetVisibleMeshObjects() const { return m_VisibleMeshObjects; }

	private:
		/* ��׶���޳����������Ե�ǰRenderView�ɼ��Ķ��� */
		void PrepareVisibleObjects();

		/* RenderView�� */
		std::string m_Name{};
		/* �ӿ����� */
		ViewportRegion m_ViewportRegion{};
		/* �Ƿ�������׶���޳� */
		bool m_IsEnableCulling{ true };
		/* ��� */
		SharedPtr<Camera> m_pCullingCamera{ nullptr };
		/* ������Scene */
		SharedPtr<Scene> m_pOwnerScene{ nullptr };
		/* ��׶���޳�֮�󣬶Ե�ǰ�ɼ�MeshSegment */
		std::vector<VisibleMeshObject> m_VisibleMeshObjects{};
		/* ��ǰView��RenderTarget Texture */
		SharedPtr<Texture> m_pRenderTargetTexture{ nullptr };
	};
}
