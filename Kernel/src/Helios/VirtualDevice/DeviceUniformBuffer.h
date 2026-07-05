#pragma once

namespace Helios
{
	/* DeviceUniformBuffer */
	class DeviceUniformBuffer
	{
	public:
		virtual ~DeviceUniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static SharedPtr<DeviceUniformBuffer> Create(uint32_t size, uint32_t binding_point);

	protected:
		DeviceUniformBuffer() = default;
	};
}


