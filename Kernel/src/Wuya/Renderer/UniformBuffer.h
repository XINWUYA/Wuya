#pragma once

namespace Wuya
{
	/* UniformBuffer */
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static SharedPtr<UniformBuffer> Create(uint32_t size, uint32_t binding_point);

	protected:
		UniformBuffer() = default;
	};
}


