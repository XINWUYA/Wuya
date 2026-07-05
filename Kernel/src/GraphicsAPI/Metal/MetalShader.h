#pragma once

#ifdef PLATFORM_MACOS

#include "Helios/Renderer/Shader.h"
#include <Metal/Metal.hpp>
#include <unordered_map>

namespace Helios
{
    class MetalShader : public Shader
    {
    public:
        MetalShader(const std::string& filepath);
        MetalShader(std::string name, const std::string& vertex_src, const std::string& pixel_src);
        ~MetalShader() override;

        void Bind() override;
        void Unbind() override;

        int GetUniformLocation(const std::string& name) override;
        void SetInt(const std::string& name, int value) override;
        void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        void SetFloat(const std::string& name, float value) override;
        void SetFloat2(const std::string& name, const glm::vec2& value) override;
        void SetFloat3(const std::string& name, const glm::vec3& value) override;
        void SetFloat4(const std::string& name, const glm::vec4& value) override;
        void SetMat4(const std::string& name, const glm::mat4& value) override;

        const std::string& GetDebugName() const override { return m_DebugName; }

        /* Metal特有接口 */
        MTL::RenderPipelineState* GetPipelineState() const { return m_PipelineState; }
        MTL::DepthStencilState* GetDepthStencilState() const { return m_DepthStencilState; }
        MTL::VertexDescriptor* GetVertexDescriptor() const { return m_VertexDescriptor; }

        void SetVertexDescriptor(MTL::VertexDescriptor* descriptor);

    private:
        struct UniformBuffer
        {
            MTL::Buffer* Buffer{ nullptr };
            uint32_t Size{ 0 };
            uint32_t Offset{ 0 };
        };

        std::string ReadFile(const std::string& filepath);
        std::string GLSLToMSL(const std::string& glsl_source, const std::string& stage);
        void CreateUniformBuffer();

        std::string m_DebugName{ "Unnamed Shader" };
        std::string m_VertexMSL;
        std::string m_FragmentMSL;
        MTL::Library* m_VertexLibrary{ nullptr };
        MTL::Library* m_FragmentLibrary{ nullptr };
        MTL::RenderPipelineState* m_PipelineState{ nullptr };
        MTL::DepthStencilState* m_DepthStencilState{ nullptr };
        MTL::VertexDescriptor* m_VertexDescriptor{ nullptr };

        /* Uniform缓冲区 */
        UniformBuffer m_UniformBuffer;
        std::unordered_map<std::string, int> m_UniformLocations;
    };
}

#endif /* PLATFORM_MACOS */