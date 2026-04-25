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
#include "GraphicsAPI/Metal/MetalUniformBuffer.h"
#include "GraphicsAPI/Metal/MetalRenderAPI.h"
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#endif

namespace Wuya
{
    /* UI uniform data for UBO */
    struct ImGuiUniformData
    {
        glm::mat4 ProjectionMatrix{ 1.0f };
    };

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

        /* 创建UI uniform buffer */
        m_UIUniformBuffer = UniformBuffer::Create(sizeof(ImGuiUniformData), 4);

        CORE_LOG_INFO("ImGuiRenderer initialized successfully");
    }

    void ImGuiRenderer::Cleanup()
    {
        m_VertexArray.reset();
        m_VertexBuffer.reset();
        m_IndexBuffer.reset();
        m_UIShader.reset();
        m_FontTexture.reset();
        m_UIUniformBuffer.reset();
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
            if (!metalShader)
            {
                CORE_LOG_ERROR("Failed to get Metal shader for ImGui rendering");
                renderEncoder->endEncoding();
                renderPassDescriptor->release();
                return;
            }
            
            /* 直接在渲染编码器上绑定着色器 */
            renderEncoder->setRenderPipelineState(metalShader->GetPipelineState());
            
            /* 绑定UI uniform buffer */
            if (m_UIUniformBuffer)
            {
                auto metalUniformBuffer = std::dynamic_pointer_cast<MetalUniformBuffer>(m_UIUniformBuffer);
                if (metalUniformBuffer && metalUniformBuffer->GetMetalBuffer())
                {
                    renderEncoder->setVertexBuffer(metalUniformBuffer->GetMetalBuffer(), 0, 4);
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
            
            /* 获取索引缓冲区 */
            auto metalIndexBuffer = std::dynamic_pointer_cast<MetalIndexBuffer>(m_IndexBuffer);
            if (!metalIndexBuffer)
            {
                CORE_LOG_ERROR("Failed to get Metal index buffer for ImGui rendering");
                renderEncoder->endEncoding();
                renderPassDescriptor->release();
                return;
            }
            
            /* 遍历所有DrawList和DrawCmd，分批次渲染 */
            index_offset = 0;
            ImTextureID lastTextureId = nullptr;
            
            for (int n = 0; n < draw_data->CmdListsCount; n++)
            {
                const ImDrawList* draw_list = draw_data->CmdLists[n];
                
                /* 遍历命令 */
                for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
                {
                    const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
                    
                    if (pcmd->UserCallback)
                    {
                        /* 用户回调 */
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
                        {
                            index_offset += pcmd->ElemCount;
                            continue;
                        }
                        
                        /* Metal坐标系Y轴向上，需要转换 */
                        MTL::ScissorRect scissorRect;
                        scissorRect.x = static_cast<NS::UInteger>(clip_min.x);
                        scissorRect.y = static_cast<NS::UInteger>(m_DisplayHeight - clip_max.y);
                        scissorRect.width = static_cast<NS::UInteger>(clip_max.x - clip_min.x);
                        scissorRect.height = static_cast<NS::UInteger>(clip_max.y - clip_min.y);
                        renderEncoder->setScissorRect(scissorRect);
                        
                        /* 根据TextureId绑定纹理 - 每个DrawCmd可能使用不同的纹理 */
                        if (pcmd->TextureId != lastTextureId)
                        {
                            MetalTexture* metalTexture = nullptr;
                            
                            /* TextureId可能存储两种类型：
                             * 1. Texture* 指针（字体纹理）
                             * 2. uint32_t 纹理ID（用户图片，从MTL::Texture*转换而来）
                             * 需要尝试两种转换方式
                             */
                            
                            /* 方式1：尝试作为Texture*指针转换（字体纹理） */
                            Texture* texture = reinterpret_cast<Texture*>(pcmd->TextureId);
                            metalTexture = dynamic_cast<MetalTexture*>(texture);
                            
                            /* 方式2：如果不是MetalTexture，尝试作为uint32_t纹理ID（用户图片） */
                            if (!metalTexture)
                            {
                                /* TextureId可能是uint32_t，代表MTL::Texture*指针 */
                                uint32_t textureId = reinterpret_cast<uintptr_t>(pcmd->TextureId);
                                if (textureId != 0)
                                {
                                    MTL::Texture* metalTexturePtr = reinterpret_cast<MTL::Texture*>(static_cast<uintptr_t>(textureId));
                                    if (metalTexturePtr)
                                    {
                                        /* 创建临时MetalTexture包装器来获取采样器状态 */
                                        /* 注意：这里无法获取原始的MetalTexture对象，所以使用默认采样器 */
                                        renderEncoder->setFragmentTexture(metalTexturePtr, 0);
                                        
                                        /* 使用默认采样器状态 - 需要从字体纹理获取 */
                                        auto defaultFontTexture = std::dynamic_pointer_cast<MetalTexture>(m_FontTexture);
                                        if (defaultFontTexture && defaultFontTexture->GetSamplerState())
                                        {
                                            renderEncoder->setFragmentSamplerState(defaultFontTexture->GetSamplerState(), 0);
                                        }
                                        lastTextureId = pcmd->TextureId;
                                        continue; /* 已经绑定纹理，跳过后续检查 */
                                    }
                                }
                            }
                            
                            /* 方式1成功：使用Texture*指针 */
                            if (metalTexture && metalTexture->GetMetalTexture())
                            {
                                renderEncoder->setFragmentTexture(metalTexture->GetMetalTexture(), 0);
                                if (metalTexture->GetSamplerState())
                                {
                                    renderEncoder->setFragmentSamplerState(metalTexture->GetSamplerState(), 0);
                                }
                                lastTextureId = pcmd->TextureId;
                            }
                            else
                            {
                                /* 所有方式都失败，使用默认字体纹理作为fallback */
                                auto defaultFontTexture = std::dynamic_pointer_cast<MetalTexture>(m_FontTexture);
                                if (defaultFontTexture && defaultFontTexture->GetMetalTexture())
                                {
                                    CORE_LOG_WARN("Unknown texture ID format, using default font texture");
                                    renderEncoder->setFragmentTexture(defaultFontTexture->GetMetalTexture(), 0);
                                    if (defaultFontTexture->GetSamplerState())
                                    {
                                        renderEncoder->setFragmentSamplerState(defaultFontTexture->GetSamplerState(), 0);
                                    }
                                    lastTextureId = pcmd->TextureId;
                                }
                                else
                                {
                                    CORE_LOG_ERROR("Failed to bind texture in ImGui rendering: texture is null or invalid");
                                }
                            }
                        }
                        
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
#else
        /* 非Mac平台：使用通用RenderAPI进行渲染（默认OpenGL） */
        auto renderAPI = Renderer::GetRenderAPI();
        if (!renderAPI)
        {
            CORE_LOG_ERROR("RenderAPI is not available for ImGui rendering");
            return;
        }

        /* 保存并配置渲染状态：ImGui需要的渲染状态（开启混合，关闭深度，关闭剔除） */
        RenderRasterState raster_state;
        raster_state.CullMode = CullMode::Cull_None;
        raster_state.EnableBlend = true;
        raster_state.BlendEquationRGB = BlendEquation::Add;
        raster_state.BlendEquationA = BlendEquation::Add;
        raster_state.BlendFuncSrcRGB = BlendFunc::SrcAlpha;
        raster_state.BlendFuncSrcA = BlendFunc::One;
        raster_state.BlendFuncDstRGB = BlendFunc::OneMinusSrcAlpha;
        raster_state.BlendFuncDstA = BlendFunc::OneMinusSrcAlpha;
        raster_state.EnableDepthWrite = false;
        raster_state.EnableColorWrite = true;
        renderAPI->ApplyRasterState(raster_state);

        /* 设置视口为整个窗口，保证ImGui能绘制到屏幕 */
        renderAPI->SetViewport(0, 0, static_cast<uint32_t>(m_DisplayWidth), static_cast<uint32_t>(m_DisplayHeight));

        /* 绑定着色器和UI uniform buffer */
        m_UIShader->Bind();
        /* 字体纹理绑定到纹理单元0（与shader里sampler2D u_FontTexture对应） */
        m_UIShader->SetInt("u_FontTexture", 0);

        /* 绑定VAO（顶点/索引缓冲已在上面填充好） */
        m_VertexArray->Bind();

        /* 遍历所有DrawList和DrawCmd，分批次渲染 */
        int local_index_offset = 0;
        ImTextureID last_texture_id = nullptr;

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* draw_list = draw_data->CmdLists[n];

            for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];

                if (pcmd->UserCallback)
                {
                    /* 用户回调 */
                    pcmd->UserCallback(draw_list, pcmd);
                }
                else
                {
                    /* 计算裁剪区域 */
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
                    {
                        local_index_offset += pcmd->ElemCount;
                        continue;
                    }

                    /* OpenGL/通用后端：Y轴向上，需要翻转 */
                    uint32_t scissor_x = static_cast<uint32_t>(clip_min.x);
                    uint32_t scissor_y = static_cast<uint32_t>(m_DisplayHeight - clip_max.y);
                    uint32_t scissor_w = static_cast<uint32_t>(clip_max.x - clip_min.x);
                    uint32_t scissor_h = static_cast<uint32_t>(clip_max.y - clip_min.y);
                    renderAPI->SetScissor(scissor_x, scissor_y, scissor_w, scissor_h);

                    /* 绑定纹理：TextureId可能是 Texture* 指针，也可能是裸纹理ID（uintptr_t） */
                    if (pcmd->TextureId != last_texture_id)
                    {
                        bool bound = false;
                        /* 先尝试作为Texture*指针（字体纹理使用这种方式） */
                        if (pcmd->TextureId)
                        {
                            Texture* texture = reinterpret_cast<Texture*>(pcmd->TextureId);
                            /* 简单健壮性处理：只要指针非空就尝试调用Bind */
                            if (texture)
                            {
                                texture->Bind(0);
                                bound = true;
                            }
                        }

                        /* fallback：使用字体纹理 */
                        if (!bound && m_FontTexture)
                        {
                            m_FontTexture->Bind(0);
                        }

                        last_texture_id = pcmd->TextureId;
                    }

                    /* 执行绘制 */
                    renderAPI->DrawIndexed(
                        PrimitiveType::Triangles,
                        m_VertexArray,
                        pcmd->ElemCount,
                        static_cast<uint32_t>(local_index_offset)
                    );
                }

                local_index_offset += pcmd->ElemCount;
            }
        }

        m_VertexArray->Unbind();
        m_UIShader->Unbind();
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

            /* 更新UBO数据 */
            if (m_UIUniformBuffer)
            {
                ImGuiUniformData data;
                data.ProjectionMatrix = m_ProjectionMatrix;
                m_UIUniformBuffer->SetData(&data, sizeof(ImGuiUniformData));
            }
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
        m_UIShader = Shader::CreateFromResource("ImGuiUI");
        
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
        
        CORE_LOG_INFO("ImGui UI shader created from embedded resource");
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
