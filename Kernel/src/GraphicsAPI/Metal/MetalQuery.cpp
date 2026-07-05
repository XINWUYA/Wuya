#ifdef PLATFORM_MACOS

#include "Pch.h"
#include "MetalQuery.h"
#include "MetalRenderAPI.h"
#include "MetalCommon.h"
#include "Helios/Renderer/Renderer.h"

#import <Metal/Metal.h>
#import <objc/message.h>

namespace Helios
{
    /* 定义 Objective-C 消息发送函数指针类型 */
    using SampleCountersFn = void (*)(id, SEL, id, NSUInteger, BOOL);
    using GetDataFn = const void* (*)(id, SEL);
    
    MetalQueryNode::MetalQueryNode()
    {
        auto device = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get())->GetDevice();
        if (!device)
        {
            CORE_LOG_ERROR("MetalQueryNode: Failed to get Metal device");
            return;
        }

        /* 创建CounterSampleBuffer用于GPU时间戳查询 */
        MTL::CounterSampleBufferDescriptor* descriptor = MTL::CounterSampleBufferDescriptor::alloc()->init();
        descriptor->setStorageMode(MTL::StorageModeShared);
        descriptor->setSampleCount(2); /* Begin和End两个时间戳 */
        
        /* 设置计数器集合为时间戳 */
        NS::Array* counterSets = device->counterSets();
        MTL::CounterSet* timestampCounterSet = nullptr;
        for (NS::UInteger i = 0; i < counterSets->count(); ++i)
        {
            MTL::CounterSet* counterSet = static_cast<MTL::CounterSet*>(counterSets->object(i));
            NS::String* name = counterSet->name();
            if (name && strcmp(name->utf8String(), "Timestamp") == 0)
            {
                timestampCounterSet = counterSet;
                break;
            }
        }

        if (!timestampCounterSet)
        {
            CORE_LOG_WARN("MetalQueryNode: Timestamp counter set not found, GPU timing disabled");
            descriptor->release();
            return;
        }

        descriptor->setCounterSet(timestampCounterSet);

        NS::Error* error = nullptr;
        m_CounterSampleBuffer = device->newCounterSampleBuffer(descriptor, &error);
        if (error || !m_CounterSampleBuffer)
        {
            CORE_LOG_WARN("MetalQueryNode: Failed to create counter sample buffer");
            if (error)
                error->release();
        }

        descriptor->release();
    }

    MetalQueryNode::~MetalQueryNode()
    {
        if (m_CounterSampleBuffer)
            m_CounterSampleBuffer->release();
    }

    void MetalQueryNode::Begin()
    {
        PROFILE_FUNCTION();

        if (!m_CounterSampleBuffer)
            return;

        auto* metalAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        auto* commandBuffer = metalAPI->GetCurrentCommandBuffer();
        if (!commandBuffer)
            return;

        m_SampleIndex = 0;
        m_HasBegin = true;
        
        /* 使用 Objective-C 运行时调用 sampleCountersInBuffer:atSampleIndex:withBarrier: */
        id mtlCommandBuffer = (__bridge id)commandBuffer;
        id mtlCounterSampleBuffer = (__bridge id)m_CounterSampleBuffer;
        
        SampleCountersFn sampleCounters = (SampleCountersFn)objc_msgSend;
        sampleCounters(mtlCommandBuffer, @selector(sampleCountersInBuffer:atSampleIndex:withBarrier:),
            mtlCounterSampleBuffer, (NSUInteger)m_SampleIndex, YES);
    }

    void MetalQueryNode::End()
    {
        PROFILE_FUNCTION();

        if (!m_CounterSampleBuffer)
            return;

        auto* metalAPI = dynamic_cast<MetalRenderAPI*>(Renderer::GetRenderAPI().get());
        auto* commandBuffer = metalAPI->GetCurrentCommandBuffer();
        if (!commandBuffer)
            return;

        m_SampleIndex = 1;
        m_HasEnd = true;
        
        /* 使用 Objective-C 运行时调用 sampleCountersInBuffer:atSampleIndex:withBarrier: */
        id mtlCommandBuffer = (__bridge id)commandBuffer;
        id mtlCounterSampleBuffer = (__bridge id)m_CounterSampleBuffer;
        
        SampleCountersFn sampleCounters = (SampleCountersFn)objc_msgSend;
        sampleCounters(mtlCommandBuffer, @selector(sampleCountersInBuffer:atSampleIndex:withBarrier:),
            mtlCounterSampleBuffer, (NSUInteger)m_SampleIndex, YES);
    }

    bool MetalQueryNode::GetQueryResult()
    {
        PROFILE_FUNCTION();

        if (!m_CounterSampleBuffer || !m_HasBegin || !m_HasEnd)
            return false;

        /* 使用 Objective-C 运行时获取数据指针 */
        id mtlCounterSampleBuffer = (__bridge id)m_CounterSampleBuffer;
        
        /* 获取数据指针 */
        GetDataFn getData = (GetDataFn)objc_msgSend;
        const uint64_t* sampleData = (const uint64_t*)getData(mtlCounterSampleBuffer, @selector(data));
        
        if (!sampleData)
            return false;

        /* 读取时间戳数据 */
        uint64_t samples[2] = {sampleData[0], sampleData[1]};

        QueryTimeBegin = samples[0];
        QueryTimeEnd = samples[1];

        /* 转换为微秒 (假设时间戳单位为纳秒) */
        ResultTimeBegin = static_cast<double>(QueryTimeBegin) * 1e-3; /* us */
        ResultTimeEnd = static_cast<double>(QueryTimeEnd) * 1e-3;     /* us */

        return true;
    }
}

#endif /* PLATFORM_MACOS */
