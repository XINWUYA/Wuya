#pragma once

#ifdef PLATFORM_MACOS

#include "Wuya/Renderer/RenderQuery.h"
#include <Metal/Metal.hpp>

namespace Wuya
{
    class MetalQueryNode final : public RenderQueryNode
    {
    public:
        MetalQueryNode();
        ~MetalQueryNode() override;

        void Begin() override;
        void End() override;
        bool GetQueryResult() override;

    private:
        /* Metal使用CounterSampleBuffer来获取GPU时间戳 */
        MTL::CounterSampleBuffer* m_CounterSampleBuffer{ nullptr };
        uint32_t m_SampleIndex{ 0 };
        bool m_HasBegin{ false };
        bool m_HasEnd{ false };
    };
}

#endif /* PLATFORM_MACOS */
