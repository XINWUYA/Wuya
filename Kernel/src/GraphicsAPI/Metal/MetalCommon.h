#pragma once

#ifdef PLATFORM_MACOS

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <glm/glm.hpp>
#include <string>
#include "Helios/Core/Logger.h"
#include "Helios/Renderer/Buffer.h"
#include "Helios/Renderer/RenderCommon.h"

namespace Helios
{
    /* Metal错误检查宏 */
    #define METAL_CHECK_ERROR(result, msg) \
        if (result) { \
            CORE_LOG_ERROR("Metal Error: {} - {}", msg, result->localizedDescription()->utf8String()); \
        }

    /* Metal对象创建检查宏 */
    #define METAL_CHECK_OBJECT(obj, msg) \
        if (!obj) { \
            CORE_LOG_ERROR("Metal Error: Failed to create object - {}", msg); \
        }

    /* Metal函数调用检查宏（带函数名） */
    #define METAL_CHECK_CALL(expr, func_name) \
        do { \
            NS::Error* error = nullptr; \
            expr; \
            if (error) { \
                CORE_LOG_ERROR("Metal Error in {}: {}", func_name, error->localizedDescription()->utf8String()); \
            } \
        } while(0)

#ifdef DEBUG
    /* 调试模式：启用Metal验证层 */
    #define METAL_ENABLE_DEBUG_LAYER(device) \
        do { \
            if (device) { \
                CORE_LOG_INFO("Metal Debug Layer enabled"); \
            } \
        } while(0)
#else
    #define METAL_ENABLE_DEBUG_LAYER(device)
#endif

    /* 将glm::vec4转换为MTL::ClearColor */
    inline MTL::ClearColor ToMetalClearColor(const glm::vec4& color)
    {
        return MTL::ClearColor(color.r, color.g, color.b, color.a);
    }

    /* 将PrimitiveType转换为MTL::PrimitiveType */
    inline MTL::PrimitiveType ToMetalPrimitiveType(PrimitiveType type)
    {
        switch (type)
        {
        case PrimitiveType::Points:
            return MTL::PrimitiveTypePoint;
        case PrimitiveType::Lines:
            return MTL::PrimitiveTypeLine;
        case PrimitiveType::Line_Strip:
            return MTL::PrimitiveTypeLineStrip;
        case PrimitiveType::Triangles:
            return MTL::PrimitiveTypeTriangle;
        case PrimitiveType::Triangle_Strip:
            return MTL::PrimitiveTypeTriangleStrip;
        default:
            CORE_LOG_ERROR("Unknown primitive type: {}", static_cast<int>(type));
            return MTL::PrimitiveTypeTriangle;
        }
    }

    /* 将BufferDataType转换为MTL::VertexFormat */
    inline MTL::VertexFormat ToMetalVertexFormat(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::Float:
            return MTL::VertexFormatFloat;
        case BufferDataType::Float2:
            return MTL::VertexFormatFloat2;
        case BufferDataType::Float3:
            return MTL::VertexFormatFloat3;
        case BufferDataType::Float4:
            return MTL::VertexFormatFloat4;
        case BufferDataType::Int:
            return MTL::VertexFormatInt;
        case BufferDataType::Int2:
            return MTL::VertexFormatInt2;
        case BufferDataType::Int3:
            return MTL::VertexFormatInt3;
        case BufferDataType::Int4:
            return MTL::VertexFormatInt4;
        case BufferDataType::Mat3:
            return MTL::VertexFormatFloat3; /* Mat3需要3个Float3 */
        case BufferDataType::Mat4:
            return MTL::VertexFormatFloat4; /* Mat4需要4个Float4 */
        case BufferDataType::Bool:
            return MTL::VertexFormatInt;
        case BufferDataType::UByte4:
            return MTL::VertexFormatUChar4Normalized; /* 4 bytes normalized to [0.0, 1.0] */
        default:
            CORE_LOG_ERROR("Unknown buffer data type: {}", static_cast<int>(type));
            return MTL::VertexFormatFloat;
        }
    }

    /* 获取BufferDataType的分量数量 */
    inline uint32_t GetComponentCount(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::Float:
        case BufferDataType::Int:
        case BufferDataType::Bool:
            return 1;
        case BufferDataType::Float2:
        case BufferDataType::Int2:
            return 2;
        case BufferDataType::Float3:
        case BufferDataType::Int3:
            return 3;
        case BufferDataType::Float4:
        case BufferDataType::Int4:
        case BufferDataType::UByte4:
            return 4;
        case BufferDataType::Mat3:
            return 3;
        case BufferDataType::Mat4:
            return 4;
        default:
            return 1;
        }
    }

    /* 获取BufferDataType的字节大小 */
    inline uint32_t GetBufferDataTypeSize(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::Float:
            return 4;
        case BufferDataType::Float2:
            return 8;
        case BufferDataType::Float3:
            return 12;
        case BufferDataType::Float4:
            return 16;
        case BufferDataType::Int:
            return 4;
        case BufferDataType::Int2:
            return 8;
        case BufferDataType::Int3:
            return 12;
        case BufferDataType::Int4:
            return 16;
        case BufferDataType::Bool:
            return 1;
        case BufferDataType::Mat3:
            return 36;
        case BufferDataType::Mat4:
            return 64;
        case BufferDataType::UByte4:
            return 4;
        default:
            return 0;
        }
    }
}

#endif /* PLATFORM_MACOS */
