#pragma once

namespace Wuya
{
	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static SharedPtr<UniformBuffer> Create(uint32_t size, uint32_t binding_point);
	};
}


