#include "Pch.h"
#include "ImGuiRenderer.h"
#include <imgui.h>

#include "glm/gtc/type_ptr.hpp"
#include "Wuya/Renderer/Renderer.h"
#include "Wuya/Renderer/RenderAPI.h"

#ifdef PLATFORM_MACOS
#include "GraphicsAPI/Metal/MetalShader.h"
#include "GraphicsAPI/Metal/MetalTexture.h"
#include "GraphicsAPI/Metal/MetalBuffer.h"
#include "GraphicsAPI/Metal/MetalVertexArray.h"
#include "GraphicsAPI/Metal/MetalRenderAPI.h"
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#endif

namespace Wuya
{
    /* UI着色器源码 - 使用Wuya的#pragma格式 */
    static const char* UI_SHADER_GLSL = R"(
#pragma vertex
#version 410 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 u_Projection;

void main()
{
    gl_Position = u_Projection * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
}

#pragma fragment
#version 410 core
in vec2 v_TexCoord;
in vec4 v_Color;

out vec4 FragColor;

uniform sampler2D u_FontTexture;

void main()
{
    vec4 texColor = texture(u_FontTexture, v_TexCoord);
    FragColor = v_Color * texColor;
}
)";

    ImGuiRenderer::ImGuiRenderer()
    {
    }

    ImGuiRenderer::~ImGuiRenderer()
    {
        Cleanup();
    }

    void ImGuiRenderer::Init()
    {
        /* 创建顶点数组 - 必须先创建 */
        m_VertexArray = VertexArray::Create();

        /* 创建初始缓冲区 - 必须在创建着色器之前 */
        EnsureBuffersCapacity(1000, 2000);

        /* 创建UI着色器 - 需要在VertexBuffer设置好布局后创建 */
        CreateUIShader();

        /* 创建字体纹理 */
        CreateFontTexture();

        CORE_LOG_INFO("ImGuiRenderer initialized successfully");
    }

    void ImGuiRenderer::Cleanup()
    {
        m_VertexArray.reset();
        m_VertexBuffer.reset();
        m_IndexBuffer.reset();
        m_UIShader.reset();
        m_FontTexture.reset();
    }

    void ImGuiRenderer::NewFrame(float delta_time)
    {
        /* 更新投影矩阵 - 使用正交投影 */
        UpdateProjectionMatrix();
    }

    void ImGuiRenderer::RenderDrawData(ImDrawData* draw_data)
    {
        if (!draw_data || draw_data->DisplaySize.x <= 0 || draw_data->DisplaySize.y <= 0)
            return;

        /* 更新显示尺寸 */
        SetDisplaySize(
            static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x),
            static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y),
            draw_data->FramebufferScale.x,
            draw_data->FramebufferScale.y
        );

        /* 计算总顶点和索引数量 */
        int total_vertex_count = 0;
        int total_index_count = 0;
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* draw_list = draw_data->CmdLists[n];
            total_vertex_count += draw_list->VtxBuffer.Size;
            total_index_count += draw_list->IdxBuffer.Size;
        }
        
        if (total_vertex_count == 0 || total_index_count == 0)
            return;

        /* 确保缓冲区足够大 */
        EnsureBuffersCapacity(total_vertex_count, total_index_count);

        /* 填充顶点和索引数据 */
        int vertex_offset = 0;
        int index_offset = 0;

        auto* vtx_data = new ImDrawVert[total_vertex_count];
        auto* idx_data = new ImDrawIdx[total_index_count];

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* draw_list = draw_data->CmdLists[n];
            
            /* 复制顶点数据 */
            memcpy(vtx_data + vertex_offset, draw_list->VtxBuffer.Data, draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
            
            /* 复制索引数据 - 需要调整索引值，加上顶点偏移 */
            for (int i = 0; i < draw_list->IdxBuffer.Size; i++)
            {
                idx_data[index_offset + i] = draw_list->IdxBuffer.Data[i] + vertex_offset;
            }
            
            vertex_offset += draw_list->VtxBuffer.Size;
            index_offset += draw_list->IdxBuffer.Size;
        }

        /* 上传顶点数据 */
        m_VertexBuffer->SetData(vtx_data, total_vertex_count * sizeof(ImDrawVert));
        
        /* 上传索引数据 - ImDrawIdx是uint16，直接使用uint16索引缓冲区 */
        m_IndexBuffer = IndexBuffer::Create(idx_data, total_index_count);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        delete[] vtx_data;
        delete[] idx_data;

#ifdef PLATFORM_MACOS
        /* 对于Metal后端，复用已有的CommandBuffer，创建独立的渲染通道 */
        auto metalRenderAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        if (metalRenderAPI)
        {
            bool createdNewCommandBuffer = false;
            
            /* 使用已有的命令缓冲区，如果没有则创建新的 */
            MTL::CommandBuffer* commandBuffer = metalRenderAPI->GetCurrentCommandBuffer();
            if (!commandBuffer)
            {
                /* 创建新的命令缓冲区 */
                MTL::CommandQueue* commandQueue = metalRenderAPI->GetCommandQueue();
                if (!commandQueue)
                {
                    CORE_LOG_ERROR("No command queue available for ImGui rendering");
                    return;
                }
                commandBuffer = commandQueue->commandBuffer();
                if (!commandBuffer)
                {
                    CORE_LOG_ERROR("Failed to create command buffer for ImGui rendering");
                    return;
                }
                createdNewCommandBuffer = true;
            }
            
            /* 创建ImGui专用的渲染通道描述符 */
            MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
            auto colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
            colorAttachment->setTexture(metalRenderAPI->GetCurrentDrawable()->texture());
            colorAttachment->setLoadAction(MTL::LoadActionLoad);  // 加载已有内容
            colorAttachment->setStoreAction(MTL::StoreActionStore);
            
            /* 注意：ImGui是2D UI，不需要深度附件，所以不设置depthAttachment */
            
            /* 创建渲染命令编码器 */
            MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
            
            /* 设置视口 */
            MTL::Viewport viewport;
            viewport.originX = 0.0;
            viewport.originY = 0.0;
            viewport.width = static_cast<double>(m_DisplayWidth);
            viewport.height = static_cast<double>(m_DisplayHeight);
            viewport.znear = 0.0;
            viewport.zfar = 1.0;
            renderEncoder->setViewport(viewport);

            /* 绑定着色器 */
            auto metalShader = std::dynamic_pointer_cast<MetalShader>(m_UIShader);
            if (metalShader)
            {
                /* 直接在渲染编码器上绑定着色器 */
                renderEncoder->setRenderPipelineState(metalShader->GetPipelineState());
                
                /* 设置投影矩阵 - 使用glm::value_ptr获取数据指针，绑定到buffer(1) */
                renderEncoder->setVertexBytes(glm::value_ptr(m_ProjectionMatrix), sizeof(glm::mat4), 1);
                
                /* 绑定字体纹理和采样器 */
                auto metalTexture = std::dynamic_pointer_cast<MetalTexture>(m_FontTexture);
                
                if (metalTexture)
                {
                    renderEncoder->setFragmentTexture(metalTexture->GetMetalTexture(), 0);
                    /* 使用纹理自带的采样器状态 */
                    if (metalTexture->GetSamplerState())
                    {
                        renderEncoder->setFragmentSamplerState(metalTexture->GetSamplerState(), 0);
                    }
                }
            }

            /* 设置渲染状态 */
            /* ImGui是2D UI，不需要深度测试 - 不设置深度模板状态，使用默认值（禁用深度测试） */
            
            /* 设置裁剪测试启用 */
            renderEncoder->setCullMode(MTL::CullModeNone);
            
            /* 绑定顶点缓冲区 - 必须显式绑定！ */
            auto metalVertexArray = std::dynamic_pointer_cast<MetalVertexArray>(m_VertexArray);
            if (metalVertexArray)
            {
                metalVertexArray->Bind(renderEncoder);
            }
            
            vertex_offset = 0;
            index_offset = 0;
            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* draw_list = draw_data->CmdLists[n];
                
                /* 绑定索引缓冲区 */
                auto metalIndexBuffer = std::dynamic_pointer_cast<MetalIndexBuffer>(m_IndexBuffer);
                if (metalIndexBuffer)
                {
                    /* 遍历命令 */
                    for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
                    {
                        const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
                        
                        if (pcmd->UserCallback)
                        {
                            pcmd->UserCallback(draw_list, pcmd);
                        }
                        else
                        {
                            /* 设置裁剪区域 */
                            ImVec2 clip_off = draw_data->DisplayPos;
                            ImVec2 clip_scale = draw_data->FramebufferScale;
                            
                            ImVec2 clip_min(
                                (pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                                (pcmd->ClipRect.y - clip_off.y) * clip_scale.y
                            );
                            ImVec2 clip_max(
                                (pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                                (pcmd->ClipRect.w - clip_off.y) * clip_scale.y
                            );
                            
                            if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                                continue;
                            
                            /* Metal坐标系Y轴向上，需要转换 */
                            MTL::ScissorRect scissorRect;
                            scissorRect.x = static_cast<NS::UInteger>(clip_min.x);
                            scissorRect.y = static_cast<NS::UInteger>(m_DisplayHeight - clip_max.y);
                            scissorRect.width = static_cast<NS::UInteger>(clip_max.x - clip_min.x);
                            scissorRect.height = static_cast<NS::UInteger>(clip_max.y - clip_min.y);
                            renderEncoder->setScissorRect(scissorRect);
                            
                            /* 绘制 - 根据索引类型选择正确的IndexType和偏移计算 */
                            MTL::IndexType metalIndexType = metalIndexBuffer->GetIndexType() == IndexType::UInt16 
                                ? MTL::IndexTypeUInt16 : MTL::IndexTypeUInt32;
                            size_t indexSize = metalIndexBuffer->GetIndexType() == IndexType::UInt16 
                                ? sizeof(uint16_t) : sizeof(uint32_t);
                            
                            renderEncoder->drawIndexedPrimitives(
                                MTL::PrimitiveTypeTriangle,
                                pcmd->ElemCount,
                                metalIndexType,
                                metalIndexBuffer->GetMetalBuffer(),
                                index_offset * indexSize  /* 索引数量转换为字节偏移 */
                            );
                        }
                        
                        index_offset += pcmd->ElemCount;
                    }
                }
                
                vertex_offset += draw_list->VtxBuffer.Size;
            }

            /* 结束渲染编码器 */
            renderEncoder->endEncoding();
            
            /* 如果创建了新的命令缓冲区，需要提交它 */
            if (createdNewCommandBuffer && commandBuffer)
            {
                commandBuffer->commit();
            }
            
            /* 释放渲染通道描述符 */
            renderPassDescriptor->release();
        }
#endif
    }

    void ImGuiRenderer::SetDisplaySize(int width, int height, float scale_x, float scale_y)
    {
        m_DisplayWidth = width;
        m_DisplayHeight = height;
        m_ScaleX = scale_x;
        m_ScaleY = scale_y;
        
        /* 立即更新投影矩阵 - 确保在任何渲染调用前矩阵都是正确的 */
        UpdateProjectionMatrix();
    }

    void ImGuiRenderer::UpdateProjectionMatrix()
    {
        /* 更新投影矩阵 - 使用正交投影 */
        if (m_DisplayWidth > 0 && m_DisplayHeight > 0)
        {
            float L = 0.0f;
            float R = static_cast<float>(m_DisplayWidth) / m_ScaleX;
            float T = 0.0f;
            float B = static_cast<float>(m_DisplayHeight) / m_ScaleY;

            m_ProjectionMatrix = glm::mat4(1.0f);
            m_ProjectionMatrix[0][0] = 2.0f / (R - L);
            m_ProjectionMatrix[1][1] = 2.0f / (T - B);
            m_ProjectionMatrix[2][2] = -1.0f;
            m_ProjectionMatrix[3][0] = (R + L) / (L - R);
            m_ProjectionMatrix[3][1] = (T + B) / (B - T);
            m_ProjectionMatrix[3][3] = 1.0f;
        }
    }

    void ImGuiRenderer::CreateFontTexture()
    {
        /* 获取字体图集数据 */
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        /* 创建纹理描述 */
        TextureDesc desc{};
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::RGBA8;
		desc.Usage = TextureUsage::Sampleable;

        /* 创建纹理 */
        m_FontTexture = Texture::Create("ImGuiFontTexture", desc);

        /* 填充纹理数据 */
        PixelDesc pixel_desc{};
        pixel_desc.Format = PixelFormat::RGBA;
		pixel_desc.Type = PixelType::UnsignedByte;
        m_FontTexture->SetData(pixels, pixel_desc);

        /* 设置ImGui字体纹理ID */
        io.Fonts->TexID = reinterpret_cast<ImTextureID>(m_FontTexture.get());

        CORE_LOG_INFO("ImGui font texture created: {}x{}", width, height);
    }

    void ImGuiRenderer::CreateUIShader()
    {
        m_UIShader = Shader::Create("ImGuiUIShader", UI_SHADER_GLSL, UI_SHADER_GLSL);
        
#ifdef PLATFORM_MACOS
        /* 对于Metal后端，需要设置VertexDescriptor */
        if (m_VertexArray)
        {
            auto metal_vertex_array = std::dynamic_pointer_cast<MetalVertexArray>(m_VertexArray);
            auto metal_shader = std::dynamic_pointer_cast<MetalShader>(m_UIShader);
            if (metal_vertex_array && metal_shader)
            {
                metal_shader->SetVertexDescriptor(metal_vertex_array->GetVertexDescriptor());
            }
        }
#endif
        
        CORE_LOG_INFO("ImGui UI shader created");
    }

    void ImGuiRenderer::EnsureBuffersCapacity(int vertex_count, int index_count)
    {
        /* 顶点缓冲区 */
        if (!m_VertexBuffer || m_VertexBufferSize < vertex_count)
        {
            m_VertexBufferSize = vertex_count;
            m_VertexBuffer = VertexBuffer::Create(m_VertexBufferSize * sizeof(ImDrawVert));

            /* 设置顶点布局 */
            VertexBufferLayout layout;
            layout.EmplaceElement(BufferElement{"Position", BufferDataType::Float2});
            layout.EmplaceElement(BufferElement{"TexCoord", BufferDataType::Float2});
            layout.EmplaceElement(BufferElement{"Color", BufferDataType::UByte4, true});  // normalized
            m_VertexBuffer->SetLayout(layout);

            /* 重新创建顶点数组并添加顶点缓冲区 */
            m_VertexArray = VertexArray::Create();
            m_VertexArray->AddVertexBuffer(m_VertexBuffer);
            
#ifdef PLATFORM_MACOS
            /* 对于Metal后端，需要更新VertexDescriptor */
            if (m_UIShader)
            {
                auto metal_vertex_array = std::dynamic_pointer_cast<MetalVertexArray>(m_VertexArray);
                auto metal_shader = std::dynamic_pointer_cast<MetalShader>(m_UIShader);
                if (metal_vertex_array && metal_shader)
                {
                    metal_shader->SetVertexDescriptor(metal_vertex_array->GetVertexDescriptor());
                }
            }
#endif
        }

        /* 索引缓冲区 - 每次渲染时重新创建以适应不同大小 */
        if (!m_IndexBuffer || m_IndexBufferSize < index_count)
        {
            m_IndexBufferSize = index_count;
        }
    }

    void ImGuiRenderer::RenderDrawList(const ImDrawList* draw_list, const ImDrawData* draw_data, int vertex_offset, int& index_offset)
    {
        /* 遍历所有命令 */
        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];

            /* 设置裁剪区域 */
            if (pcmd->UserCallback)
            {
                /* 用户回调 */
                pcmd->UserCallback(draw_list, pcmd);
            }
            else
            {
                /* 设置裁剪区域 - 使用Scissor而不是Viewport */
                ImVec2 clip_off = draw_data->DisplayPos;
                ImVec2 clip_scale = draw_data->FramebufferScale;

                ImVec2 clip_min(
                    (pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
                    (pcmd->ClipRect.y - clip_off.y) * clip_scale.y
                );
                ImVec2 clip_max(
                    (pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
                    (pcmd->ClipRect.w - clip_off.y) * clip_scale.y
                );

                /* 边界检查 */
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                /* 应用裁剪区域 - Metal坐标系Y轴向上，需要转换 */
                uint32_t scissor_x = static_cast<uint32_t>(clip_min.x);
                uint32_t scissor_y = static_cast<uint32_t>(m_DisplayHeight - clip_max.y);  // 翻转Y轴
                uint32_t scissor_width = static_cast<uint32_t>(clip_max.x - clip_min.x);
                uint32_t scissor_height = static_cast<uint32_t>(clip_max.y - clip_min.y);
                
                Renderer::GetRenderAPI()->SetScissor(scissor_x, scissor_y, scissor_width, scissor_height);

                /* 绑定纹理（如果不是默认字体纹理） */
                if (pcmd->TextureId)
                {
                    auto* texture = reinterpret_cast<Texture*>(pcmd->TextureId);
                    texture->Bind(0);
                }

                /* 绘制 - 索引已经在填充时调整为全局索引，直接使用index_offset */
                m_VertexArray->Bind();
                Renderer::GetRenderAPI()->DrawIndexed(
                    PrimitiveType::Triangles,
                    m_VertexArray,
                    pcmd->ElemCount,
                    index_offset  // 使用当前索引偏移
                );
            }

            index_offset += pcmd->ElemCount;
        }
    }
}
