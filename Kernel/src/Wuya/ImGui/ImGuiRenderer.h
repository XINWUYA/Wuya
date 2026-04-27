#pragma once

#include "Wuya/Renderer/Shader.h"
#include "Wuya/Renderer/Texture.h"
#include "Wuya/Renderer/Buffer.h"
#include "Wuya/Renderer/VertexArray.h"
#include "Wuya/Renderer/UniformBuffer.h"
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

// Forward declarations for ImGui types
struct ImDrawData;
struct ImDrawList;

#ifdef PLATFORM_MACOS
// Forward declaration for Metal types
namespace MTL
{
    class SamplerState;
}
#endif

namespace Wuya
{
    /**
     * \brief ImGui渲染器 - 类似Filament的filagui设计
     * 不使用ImGui官方后端，而是通过Wuya的RHI抽象层实现跨平台渲染
     */
    class ImGuiRenderer
    {
    public:
        ImGuiRenderer();
        ~ImGuiRenderer();

        /* 初始化渲染器 */
        void Init();
        /* 释放资源 */
        void Cleanup();

        /* 开始新帧 - 更便显示尺寸等 */
        void NewFrame(float delta_time);
        /* 处理ImGui绘制数据并渲染 */
        void RenderDrawData(ImDrawData* draw_data);

        /* 设置显示尺寸 */
        void SetDisplaySize(int width, int height, float scale_x = 1.0f, float scale_y = 1.0f);

        /* 获取字体纹理 */
        SharedPtr<Texture> GetFontTexture() const { return m_FontTexture; }

    private:
        /* 创建字体图集纹理 */
        void CreateFontTexture();
        /* 创建UI着色器 */
        void CreateUIShader();
        /* 创建缓冲区 */
        void EnsureBuffersCapacity(int vertex_count, int index_count);
        /* 渲染单个DrawList */
        void RenderDrawList(const ImDrawList* draw_list, const ImDrawData* draw_data, int vertex_offset, int& index_offset);
        /* 更新投影矩阵 */
        void UpdateProjectionMatrix();

        /* 显示尺寸 */
        int m_DisplayWidth{ 0 };
        int m_DisplayHeight{ 0 };
        float m_ScaleX{ 1.0f };
        float m_ScaleY{ 1.0f };

        /* 字体纹理 */
        SharedPtr<Texture> m_FontTexture{ nullptr };

        /* UI着色器 */
        SharedPtr<Shader> m_UIShader{ nullptr };

        /* 顶点和索引缓冲区 */
        SharedPtr<VertexBuffer> m_VertexBuffer{ nullptr };
        SharedPtr<IndexBuffer> m_IndexBuffer{ nullptr };
        SharedPtr<VertexArray> m_VertexArray{ nullptr };

        /* 缓冲区容量 */
        int m_VertexBufferSize{ 0 };
        int m_IndexBufferSize{ 0 };

        /* 每帧复用的CPU暂存缓冲（以字节为单位，避免依赖ImGui类型前向声明） */
        std::vector<uint8_t> m_VtxScratch;
        std::vector<uint8_t> m_IdxScratch;

        /* UI uniform buffer */
        SharedPtr<UniformBuffer> m_UIUniformBuffer{ nullptr };

        /* 投影矩阵 */
        glm::mat4 m_ProjectionMatrix{ 1.0f };
    };
}
