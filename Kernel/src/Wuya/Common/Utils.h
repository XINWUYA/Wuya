#pragma once
#include <regex>
#include <string>
#include <glm/glm.hpp>

namespace Wuya
{
	/* split string */
	inline std::vector<std::string> Split(const std::string& value, const std::string& delimiter)
	{
		const std::regex regex(delimiter);
		std::vector<std::string> result(std::sregex_token_iterator(value.begin(), value.end(), regex, -1), std::sregex_token_iterator());
		return result;
	}

	/* vec3 to string */
	inline std::string ToString(const glm::vec3& value)
	{
		return std::to_string(value.x) + " " + std::to_string(value.y) + " " + std::to_string(value.z);
	}

	/* vec4 to string */
	inline std::string ToString(const glm::vec4& value)
	{
		return std::to_string(value.x) + " " + std::to_string(value.y) + " " + std::to_string(value.z) + " " + std::to_string(value.w);
	}

	/* string to vec3
	 * not very safe, need make sure the value is valid
	 */
	inline glm::vec3 ToVec3(const std::string& value)
	{
		if (value.empty())
			return { 0.0f, 0.0f, 0.0f };

		const std::vector<std::string> split_result = Split(value, " ");
		return {std::stof(split_result[0]), std::stof(split_result[1]), std::stof(split_result[2])};
	}

	/* string to vec4
	 * not very safe, need make sure the value is valid
	 */
	inline glm::vec4 ToVec4(const std::string& value)
	{
		if (value.empty())
			return { 0.0f, 0.0f, 0.0f, 0.0f };

		const std::vector<std::string> split_result = Split(value, " ");
		return { std::stof(split_result[0]), std::stof(split_result[1]), std::stof(split_result[2]), std::stof(split_result[3]) };
	}

	/* Enum类型的位运算 */
	template<typename Enum>
	struct EnableBitmaskOperators : public std::false_type
	{ };

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr bool operator!(Enum e) noexcept
	{
		using underlying = std::underlying_type_t<Enum>;
		return underlying(e) == 0;
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator~(Enum e) noexcept
	{
		using underlying = std::underlying_type_t<Enum>;
		return Enum(~underlying(e));
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator|(Enum lt, Enum rt) noexcept
	{
		using underlying = std::underlying_type_t<Enum>;
		return Enum(underlying(lt) | underlying(rt));
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator&(Enum lt, Enum rt) noexcept
	{
		using underlying = std::underlying_type_t<Enum>;
		return Enum(underlying(lt) & underlying(rt));
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator^(Enum lt, Enum rt) noexcept
	{
		using underlying = std::underlying_type_t<Enum>;
		return Enum(underlying(lt) ^ underlying(rt));
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator|=(Enum& lt, Enum rt) noexcept
	{
		return lt = lt | rt;
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator&=(Enum& lt, Enum rt) noexcept
	{
		return lt = lt & rt;
	}

	template<typename Enum, typename std::enable_if_t<std::is_enum<Enum>::value && EnableBitmaskOperators<Enum>::value, int> = 0>
	inline constexpr Enum operator^=(Enum& lt, Enum rt) noexcept
	{
		return lt = lt ^ rt;
	}
}
