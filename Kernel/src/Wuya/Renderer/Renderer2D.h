#pragma once
#include <glm/glm.hpp>

namespace Wuya
{
	class VertexArray;
	class Texture2D;
	class Shader;

	class Renderer2D
	{
	public:
		static void Init();
		static void Update();
		static void Release();

		static void BeginScene(const SharedPtr<class Camera>& camera);
		static void EndScene();

		/* Draw Quads */
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const SharedPtr<Texture2D>& texture, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float tiling_factor = 1.0f);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entity_id = -1);
		static void DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tiling_factor = 1.0f, int entity_id = -1);

		/* 绘制一个2D图片 */
		static void DrawSprite(const glm::mat4& transform, const struct SpriteComponent* component, int entity_id);

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
		static void StartNewBatch();
		static void NextBatch();
		static void Flush();

	};
}
