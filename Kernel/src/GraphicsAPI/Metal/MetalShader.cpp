#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalShader.h"
#include "MetalCommon.h"
#include "MetalRenderAPI.h"
#include "Helios/Renderer/Renderer.h"

namespace Helios
{
    MetalShader::MetalShader(const std::string& filepath)
        : DeviceShader(filepath)
    {
        /* 从文件路径提取着色器名称 */
        auto last_slash = filepath.find_last_of("/\\");
        auto last_dot = filepath.find_last_of('.');
        if (last_slash != std::string::npos && last_dot != std::string::npos)
            m_DebugName = filepath.substr(last_slash + 1, last_dot - last_slash - 1);
        else
            m_DebugName = filepath;

        /* 读取着色器源码 */
        std::string source = ReadFile(filepath);

        /* 解析GLSL源码，分离顶点和片段着色器 */
        std::string vertex_src, fragment_src;
        size_t vertex_pos = source.find("#pragma vertex");
        size_t fragment_pos = source.find("#pragma fragment");

        if (vertex_pos != std::string::npos && fragment_pos != std::string::npos)
        {
            vertex_src = source.substr(vertex_pos, fragment_pos - vertex_pos);
            fragment_src = source.substr(fragment_pos);
        }
        else
        {
            CORE_LOG_ERROR("Invalid shader format: missing #pragma vertex or #pragma fragment");
            return;
        }

        /* 转换GLSL到MSL */
        m_VertexMSL = GLSLToMSL(vertex_src, "vertex");
        m_FragmentMSL = GLSLToMSL(fragment_src, "fragment");

        /* 创建Uniform缓冲区 */
        CreateUniformBuffer();

        CORE_LOG_INFO("Metal shader created: {}", m_DebugName);
    }

    MetalShader::MetalShader(std::string name, const std::string& vertex_src, const std::string& pixel_src)
        : DeviceShader(""), m_DebugName(std::move(name))
    {
        /* 转换GLSL到MSL */
        m_VertexMSL = GLSLToMSL(vertex_src, "vertex");
        m_FragmentMSL = GLSLToMSL(pixel_src, "fragment");

        /* 创建Uniform缓冲区 */
        CreateUniformBuffer();

        CORE_LOG_INFO("Metal shader created: {}", m_DebugName);
    }

    MetalShader::~MetalShader()
    {
        if (m_VertexLibrary)
            m_VertexLibrary->release();
        if (m_FragmentLibrary)
            m_FragmentLibrary->release();
        if (m_PipelineState)
            m_PipelineState->release();
        if (m_DepthStencilState)
            m_DepthStencilState->release();
        if (m_VertexDescriptor)
            m_VertexDescriptor->release();
        if (m_UniformBuffer.Buffer)
            m_UniformBuffer.Buffer->release();
    }

    void MetalShader::Bind()
    {
        /* Metal中需要在render encoder上设置PipelineState */
        auto metalRenderAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        if (metalRenderAPI && m_PipelineState)
        {
            auto encoder = metalRenderAPI->GetCurrentRenderEncoder();
            if (encoder)
            {
                encoder->setRenderPipelineState(m_PipelineState);
                
                /* 绑定Uniform缓冲区 */
                if (m_UniformBuffer.Buffer)
                {
                    encoder->setVertexBuffer(m_UniformBuffer.Buffer, 0, 0);
                    encoder->setFragmentBuffer(m_UniformBuffer.Buffer, 0, 0);
                }
            }
        }
    }

    void MetalShader::Unbind()
    {
        /* Metal中无需解绑 */
    }

    int MetalShader::GetUniformLocation(const std::string& name)
    {
        auto it = m_UniformLocations.find(name);
        if (it != m_UniformLocations.end())
            return it->second;
        return -1;
    }

    void MetalShader::SetInt(const std::string& name, int value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(int));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(int)));
        }
    }

    void MetalShader::SetIntArray(const std::string& name, int* values, uint32_t count)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, values, count * sizeof(int));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, count * sizeof(int)));
        }
    }

    void MetalShader::SetFloat(const std::string& name, float value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(float));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(float)));
        }
    }

    void MetalShader::SetFloat2(const std::string& name, const glm::vec2& value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(glm::vec2));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(glm::vec2)));
        }
    }

    void MetalShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(glm::vec3));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(glm::vec3)));
        }
    }

    void MetalShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(glm::vec4));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(glm::vec4)));
        }
    }

    void MetalShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        int location = GetUniformLocation(name);
        if (location >= 0 && m_UniformBuffer.Buffer)
        {
            void* data = m_UniformBuffer.Buffer->contents();
            memcpy(static_cast<char*>(data) + location, &value, sizeof(glm::mat4));
            m_UniformBuffer.Buffer->didModifyRange(NS::Range(location, sizeof(glm::mat4)));
        }
    }

    void MetalShader::SetVertexDescriptor(MTL::VertexDescriptor* descriptor)
    {
        if (m_VertexDescriptor)
            m_VertexDescriptor->release();
        m_VertexDescriptor = descriptor;
        if (m_VertexDescriptor)
        {
            m_VertexDescriptor->retain();
            
            /* 如果MSL源码已准备好，创建着色器库和管线状态 */
            if (!m_VertexMSL.empty() && !m_FragmentMSL.empty())
            {
                /* 释放旧的资源 */
                if (m_VertexLibrary)
                {
                    m_VertexLibrary->release();
                    m_VertexLibrary = nullptr;
                }
                if (m_FragmentLibrary)
                {
                    m_FragmentLibrary->release();
                    m_FragmentLibrary = nullptr;
                }
                if (m_PipelineState)
                {
                    m_PipelineState->release();
                    m_PipelineState = nullptr;
                }
                if (m_DepthStencilState)
                {
                    m_DepthStencilState->release();
                    m_DepthStencilState = nullptr;
                }
                
                /* 重新创建着色器库和管线状态 */
                auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
                if (!device)
                {
                    CORE_LOG_ERROR("Metal device is null!");
                    return;
                }

                /* 编译顶点着色器 */
                NS::Error* error = nullptr;
                NS::String* vertex_src = NS::String::string(m_VertexMSL.c_str(), NS::UTF8StringEncoding);
                m_VertexLibrary = device->newLibrary(vertex_src, nullptr, &error);
                if (error || !m_VertexLibrary)
                {
                    CORE_LOG_ERROR("Failed to compile vertex shader: {}", 
                        error ? error->localizedDescription()->utf8String() : "Unknown error");
                    return;
                }

                /* 编译片段着色器 */
                NS::String* fragment_src = NS::String::string(m_FragmentMSL.c_str(), NS::UTF8StringEncoding);
                m_FragmentLibrary = device->newLibrary(fragment_src, nullptr, &error);
                if (error || !m_FragmentLibrary)
                {
                    CORE_LOG_ERROR("Failed to compile fragment shader: {}",
                        error ? error->localizedDescription()->utf8String() : "Unknown error");
                    return;
                }

                /* 创建渲染管线描述符 */
                MTL::RenderPipelineDescriptor* pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
                pipelineDescriptor->setLabel(NS::String::string(m_DebugName.c_str(), NS::UTF8StringEncoding));
                
                /* 设置顶点函数 */
                MTL::Function* vertexFunction = m_VertexLibrary->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
                pipelineDescriptor->setVertexFunction(vertexFunction);
                
                /* 设置片段函数 */
                MTL::Function* fragmentFunction = m_FragmentLibrary->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
                pipelineDescriptor->setFragmentFunction(fragmentFunction);
                
                /* 设置颜色附件格式 */
                pipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
                
                /* 设置顶点描述符 */
                if (m_VertexDescriptor)
                    pipelineDescriptor->setVertexDescriptor(m_VertexDescriptor);

                /* 创建渲染管线状态 */
                m_PipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);
                if (error || !m_PipelineState)
                {
                    CORE_LOG_ERROR("Failed to create pipeline state: {}",
                        error ? error->localizedDescription()->utf8String() : "Unknown error");
                }

                /* 创建深度模板状态 */
                MTL::DepthStencilDescriptor* depthDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
                depthDescriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
                depthDescriptor->setDepthWriteEnabled(true);
                m_DepthStencilState = device->newDepthStencilState(depthDescriptor);

                /* 释放临时对象 */
                vertexFunction->release();
                fragmentFunction->release();
                pipelineDescriptor->release();
                depthDescriptor->release();
            }
        }
    }

    std::string MetalShader::ReadFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            CORE_LOG_ERROR("Could not open shader file: {}", filepath);
            return "";
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    std::string MetalShader::GLSLToMSL(const std::string& glsl_source, const std::string& stage)
    {
        /* 这里提供一个简单的GLSL到MSL转换示例 */
        /* 实际项目中应该使用SPIRV-Cross进行完整转换 */
        
        std::string msl_source;
        
        /* 添加MSL头文件 */
        msl_source = "#include <metal_stdlib>\nusing namespace metal;\n\n";
        
        /* 简单的着色器转换（实际需要更复杂的解析） */
        if (stage == "vertex")
        {
            msl_source += "struct VertexIn {\n";
            msl_source += "    float2 position [[attribute(0)]];\n";
            msl_source += "    float2 texCoord [[attribute(1)]];\n";
            msl_source += "    float4 color [[attribute(2)]];\n";
            msl_source += "};\n\n";
            msl_source += "struct VertexOut {\n";
            msl_source += "    float4 position [[position]];\n";
            msl_source += "    float2 texCoord;\n";
            msl_source += "    float4 color;\n";
            msl_source += "};\n\n";
            msl_source += "vertex VertexOut vertex_main(VertexIn in [[stage_in]], constant float4x4& u_Projection [[buffer(1)]]) {\n";
            msl_source += "    VertexOut out;\n";
            msl_source += "    out.position = u_Projection * float4(in.position, 0.0, 1.0);\n";
            msl_source += "    out.texCoord = in.texCoord;\n";
            msl_source += "    out.color = in.color;\n";
            msl_source += "    return out;\n";
            msl_source += "}\n";
        }
        else if (stage == "fragment")
        {
            msl_source += "struct FragmentIn {\n";
            msl_source += "    float4 position [[position]];\n";
            msl_source += "    float2 texCoord;\n";
            msl_source += "    float4 color;\n";
            msl_source += "};\n\n";
            msl_source += "fragment float4 fragment_main(FragmentIn in [[stage_in]], texture2d<float> u_FontTexture [[texture(0)]], sampler u_FontTextureSampler [[sampler(0)]]) {\n";
            msl_source += "    float4 texColor = u_FontTexture.sample(u_FontTextureSampler, in.texCoord);\n";
            msl_source += "    return in.color * texColor;\n";
            msl_source += "}\n";
        }

        return msl_source;
    }



    void MetalShader::CreateUniformBuffer()
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
            return;

        /* 创建一个足够大的Uniform缓冲区 */
        m_UniformBuffer.Size = 4096; /* 4KB */
        m_UniformBuffer.Buffer = device->newBuffer(m_UniformBuffer.Size, MTL::ResourceOptionCPUCacheModeDefault);
    }
}

#endif /* PLATFORM_MACOS */