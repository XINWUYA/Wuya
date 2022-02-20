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

	private:
		static void Flush();

	};
}
