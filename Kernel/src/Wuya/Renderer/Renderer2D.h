#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class VertexArray;
	class Texture2D;
	class Shader;
	class OrthographicCamera;

	class Renderer2D
	{
	public:
		static void Init();
		static void Update();
		static void Release();

		static void BeginScene(SharedPtr<OrthographicCamera> camera);
		static void EndScene();

		// Draw Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1);

		// 统计信息
		struct StatisticsInfo
		{
			uint32_t DrawCalls{ 0 };
			uint32_t QuadCount{ 0 };

			uint32_t TotalVertexCount() const { return QuadCount * 4; }
			uint32_t TotalIndexCount() const { return QuadCount * 6; }
		};

		static StatisticsInfo GetStatisticsInfo();
		static void ResetStatisticsInfo();

	private:
		static void Flush();

	};
}
