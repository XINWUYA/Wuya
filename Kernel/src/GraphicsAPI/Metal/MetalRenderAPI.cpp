#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalRenderAPI.h"
#include "MetalCommon.h"
#include "MetalVertexArray.h"
#include "Helios/Renderer/VertexArray.h"

namespace Helios
{
    MetalRenderAPI::~MetalRenderAPI()
    {
        if (m_CommandQueue)
            m_CommandQueue->release();
        if (m_Device)
            m_Device->release();
    }

    void MetalRenderAPI::Init()
    {
        PROFILE_FUNCTION();

        /* 创建Metal设备 */
        m_Device = MTL::CreateSystemDefaultDevice();
        if (!m_Device)
        {
            CORE_LOG_ERROR("Failed to create Metal device!");
            return;
        }

        CORE_LOG_INFO("Metal Device: {}", m_Device->name()->utf8String());

#ifdef DEBUG
        /* 调试模式下输出设备信息 */
        CORE_LOG_INFO("Metal Debug Mode: Enabled");
        CORE_LOG_INFO("    Max Buffer Length: {} MB", m_Device->maxBufferLength() / (1024 * 1024));
        CORE_LOG_INFO("    Supports Raytracing: {}", m_Device->supportsRaytracing() ? "Yes" : "No");
        CORE_LOG_INFO("    Supports ShaderBarycentricCoordinates: {}", 
            m_Device->supportsShaderBarycentricCoordinates() ? "Yes" : "No");
#endif

        /* 创建命令队列 */
        m_CommandQueue = m_Device->newCommandQueue();
        if (!m_CommandQueue)
        {
            CORE_LOG_ERROR("Failed to create Metal command queue!");
            return;
        }

        CORE_LOG_INFO("MetalRenderAPI initialized successfully.");
    }

    void MetalRenderAPI::SetViewport(uint32_t x_start, uint32_t y_start, uint32_t width, uint32_t height)
    {
        PROFILE_FUNCTION();

        m_CurrentWidth = width;
        m_CurrentHeight = height;

        if (m_CurrentRenderEncoder)
        {
            MTL::Viewport viewport;
            viewport.originX = static_cast<double>(x_start);
            viewport.originY = static_cast<double>(y_start);
            viewport.width = static_cast<double>(width);
            viewport.height = static_cast<double>(height);
            viewport.znear = 0.0;
            viewport.zfar = 1.0;
            m_CurrentRenderEncoder->setViewport(viewport);
        }
    }

    void MetalRenderAPI::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        PROFILE_FUNCTION();

        if (m_CurrentRenderEncoder)
        {
            MTL::ScissorRect scissorRect;
            scissorRect.x = x;
            scissorRect.y = y;
            scissorRect.width = width;
            scissorRect.height = height;
            m_CurrentRenderEncoder->setScissorRect(scissorRect);
        }
    }

    void MetalRenderAPI::SetClearColor(const glm::vec4& color)
    {
        m_ClearColor = color;
    }

    void MetalRenderAPI::Clear()
    {
        /* Metal中清除操作在BeginRenderPass时通过配置loadAction完成 */
        /* 这里不需要额外操作 */
    }

    void MetalRenderAPI::ApplyRasterState(RenderRasterState raster_state)
    {
        PROFILE_FUNCTION();

        m_CurrentRasterState = raster_state;

        if (m_CurrentRenderEncoder)
        {
            /* 设置剔除模式 */
            switch (raster_state.CullMode)
            {
            case CullMode::Cull_None:
                m_CurrentRenderEncoder->setCullMode(MTL::CullModeNone);
                break;
            case CullMode::Cull_Front:
                m_CurrentRenderEncoder->setCullMode(MTL::CullModeFront);
                break;
            case CullMode::Cull_Back:
                m_CurrentRenderEncoder->setCullMode(MTL::CullModeBack);
                break;
            }

            /* 设置缠绕方向 */
            if (raster_state.FrontFaceType == FrontFaceType::CW)
                m_CurrentRenderEncoder->setFrontFacingWinding(MTL::WindingClockwise);
            else
                m_CurrentRenderEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
        }
    }

    void MetalRenderAPI::DrawIndexed(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array, uint32_t index_count, uint32_t index_offset)
    {
        PROFILE_FUNCTION();

        if (!m_CurrentRenderEncoder)
        {
            CORE_LOG_ERROR("No active render encoder!");
            return;
        }

        auto metal_vertex_array = std::dynamic_pointer_cast<MetalVertexArray>(vertex_array);
        if (!metal_vertex_array)
        {
            CORE_LOG_ERROR("Invalid vertex array type!");
            return;
        }

        /* 绑定顶点缓冲区 */
        metal_vertex_array->Bind(m_CurrentRenderEncoder);

        /* 获取索引缓冲区信息 */
        auto index_buffer = metal_vertex_array->GetIndexBuffer();
        if (!index_buffer)
        {
            CORE_LOG_ERROR("Index buffer is null!");
            return;
        }

        auto metal_index_buffer = std::dynamic_pointer_cast<MetalIndexBuffer>(index_buffer);
        if (!metal_index_buffer)
        {
            CORE_LOG_ERROR("Invalid index buffer type!");
            return;
        }

        uint32_t count = index_count > 0 ? index_count : index_buffer->GetCount();

        /* 执行索引绘制 - 使用index_offset */
        m_CurrentRenderEncoder->drawIndexedPrimitives(
            ToMetalPrimitiveType(type),
            count,
            MTL::IndexTypeUInt32,
            metal_index_buffer->GetMetalBuffer(),
            index_offset * sizeof(uint32_t)  // 索引偏移（字节）
        );
    }

    void MetalRenderAPI::DrawArrays(PrimitiveType type, const SharedPtr<VertexArray>& vertex_array)
    {
        PROFILE_FUNCTION();

        if (!m_CurrentRenderEncoder)
        {
            CORE_LOG_ERROR("No active render encoder!");
            return;
        }

        auto metal_vertex_array = std::dynamic_pointer_cast<MetalVertexArray>(vertex_array);
        if (!metal_vertex_array)
        {
            CORE_LOG_ERROR("Invalid vertex array type!");
            return;
        }

        /* 绑定顶点缓冲区 */
        metal_vertex_array->Bind(m_CurrentRenderEncoder);

        /* 获取顶点数量 */
        uint32_t vertex_count = metal_vertex_array->GetVertexCount();

        /* 执行顶点绘制 */
        m_CurrentRenderEncoder->drawPrimitives(
            ToMetalPrimitiveType(type),
            NS::UInteger(0),
            NS::UInteger(vertex_count)
        );
    }

    void MetalRenderAPI::Flush()
    {
        PROFILE_FUNCTION();

        /* 注意：这个方法现在只用于最终提交，不应该在每个Pass后调用 */
        /* 命令缓冲区会在Present时提交 */
    }

    void MetalRenderAPI::Present()
    {
        PROFILE_FUNCTION();

        if (m_CurrentCommandBuffer)
        {
            /* 如果有drawable，需要在提交前呈现 */
            if (m_CurrentDrawable)
            {
                m_CurrentCommandBuffer->presentDrawable(m_CurrentDrawable);
            }
            m_CurrentCommandBuffer->commit();
            m_CurrentCommandBuffer = nullptr;
        }
    }

    void MetalRenderAPI::PrepareNextDrawable()
    {
        PROFILE_FUNCTION();

        if (!m_MetalLayer)
        {
            CORE_LOG_ERROR("MetalLayer is not set!");
            return;
        }

        /* 获取下一个可绘制对象 */
        m_CurrentDrawable = m_MetalLayer->nextDrawable();
        if (!m_CurrentDrawable)
        {
            CORE_LOG_ERROR("Failed to get next drawable!");
            return;
        }

        /* 配置渲染通道描述符的颜色附件 */
        if (m_RenderPassDescriptor)
        {
            auto colorAttachment = m_RenderPassDescriptor->colorAttachments()->object(0);
            colorAttachment->setTexture(m_CurrentDrawable->texture());
            colorAttachment->setLoadAction(MTL::LoadActionClear);
            colorAttachment->setStoreAction(MTL::StoreActionStore);
            colorAttachment->setClearColor(MTL::ClearColor(
                m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a));
        }
    }

    void MetalRenderAPI::PushDebugGroup(const char* name)
    {
        if (m_CurrentRenderEncoder)
        {
            m_CurrentRenderEncoder->pushDebugGroup(NS::String::string(name, NS::UTF8StringEncoding));
        }
    }

    void MetalRenderAPI::PopDebugGroup()
    {
        if (m_CurrentRenderEncoder)
        {
            m_CurrentRenderEncoder->popDebugGroup();
        }
    }

    void MetalRenderAPI::BeginRenderPass(MTL::RenderPassDescriptor* renderPassDescriptor)
    {
        PROFILE_FUNCTION();

        if (!m_Device || !m_CommandQueue)
        {
            CORE_LOG_ERROR("Metal device or command queue not initialized!");
            return;
        }

        /* 如果没有命令缓冲区，创建一个新的 */
        if (!m_CurrentCommandBuffer)
        {
            m_CurrentCommandBuffer = m_CommandQueue->commandBuffer();
            if (!m_CurrentCommandBuffer)
            {
                CORE_LOG_ERROR("Failed to create command buffer!");
                return;
            }
        }

        /* 创建渲染命令编码器 */
        m_CurrentRenderEncoder = m_CurrentCommandBuffer->renderCommandEncoder(renderPassDescriptor);
        if (!m_CurrentRenderEncoder)
        {
            CORE_LOG_ERROR("Failed to create render command encoder!");
            return;
        }

        /* 应用当前光栅化状态 */
        ApplyRasterState(m_CurrentRasterState);
    }

    void MetalRenderAPI::EndRenderPass()
    {
        PROFILE_FUNCTION();

        if (m_CurrentRenderEncoder)
        {
            m_CurrentRenderEncoder->endEncoding();
            m_CurrentRenderEncoder = nullptr;
        }
    }
}

#endif /* PLATFORM_MACOS */
