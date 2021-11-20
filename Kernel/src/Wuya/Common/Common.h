#pragma once
#include <memory>

#define BIT(x) (1 << (x))
#define EXPAND_MACRO(x) x
#define STRINGIFY(x) #x
#define BIND_EVENT_FUNC(func) [this](auto&& ...args)->decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Wuya
{
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr UniquePtr<T> CreateUniquePtr(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename ... Args>
	constexpr SharedPtr<T> CreateSharedPtr(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}