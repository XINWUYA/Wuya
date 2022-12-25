#pragma once
#include <regex>
#include <string>
#include <glm/glm.hpp>
#include <magic_enum.hpp>

namespace Wuya
{
	/* Extract file base dir from path
	 * Example: 
	 * path: "assets/scenes/test.scn"
	 * return: "assets/scenes/"
	 */
	std::string ExtractFileBaseDir(const std::string& path);

	/* Extract file base name from path
	 * Example:
	 * path: "assets/scenes/test.scn"
	 * return: "test.scn"
	 */
	std::string ExtractFileBaseName(const std::string& path);

	/* Extract file base dir & base name from path
	 * Example:
	 * path: "assets/scenes/test.scn"
	 * return: {"assets/scenes/", "test.scn"}
	 */
	std::pair<std::string, std::string> ExtractFileBaseDirAndBaseName(const std::string& path);

	/* Extract filename from path
	 * Examples:
	 * path: "assets/scenes/test.scn"
	 * return: "test"
	 */
	std::string ExtractFilename(const std::string& path);

	/* Extract filename from path
	 * Examples:
	 * path: "assets/scenes/test.scn"
	 * return: "scn"
	 */
	std::string ExtractFileSuffix(const std::string& path);

	/* Replace file suffix
	 * Examples:
	 * path: "assets/scenes/test.scn", suffix: "bin"
	 * return: "assets/scenes/test.bin"
	 */
	std::string ReplaceFileSuffix(const std::string& path, const std::string& suffix);

	/* split string */
	inline std::vector<std::string> Split(const std::string& value, const std::string& delimiter)
	{
		const std::regex regex(delimiter);
		std::vector<std::string> result(std::sregex_token_iterator(value.begin(), value.end(), regex, -1), std::sregex_token_iterator());
		return result;
	}

	/* float to string */
	inline std::string ToString(float value, int precision = 6)
	{
		if (precision < 0)
			return std::to_string(value);

		std::ostringstream oss;
		oss.precision(precision);
		oss << std::fixed << value;
		return oss.str();
	}

	/* vec2 to string */
	inline std::string ToString(const glm::vec2& value, int precision = 6)
	{
		return ToString(value.x, precision) + " " + ToString(value.y, precision);
	}

	/* vec3 to string */
	inline std::string ToString(const glm::vec3& value, int precision = 6)
	{
		return ToString(value.x, precision) + " " + ToString(value.y, precision) + " " + ToString(value.z, precision);
	}

	/* vec4 to string */
	inline std::string ToString(const glm::vec4& value, int precision = 6)
	{
		return ToString(value.x, precision) + " " + ToString(value.y, precision) + " " + ToString(value.z, precision) + " " + ToString(value.w, precision);
	}

	/* string to vec2
	 * not very safe, need make sure the value is valid
	 */
	inline glm::vec2 ToVec2(const std::string& value)
	{
		if (value.empty())
			return { 0.0f, 0.0f };

		const std::vector<std::string> split_result = Split(value, " ");
		return { std::stof(split_result[0]), std::stof(split_result[1]) };
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

	/* string to Id
	 * use xxHash
	 */
	uint32_t ToID(const std::string& value);

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

	/* 获取Enum的所有名称 */
	template <typename Enum>
	inline std::vector<std::string> GetEnumNames()
	{
		constexpr auto& enum_names = magic_enum::enum_names<Enum>();

		std::vector<std::string> result;
		result.reserve(enum_names.size());
		for (auto& mode : enum_names)
			result.emplace_back(mode);

		return result;
	}

	/* 获取单个Enum的名称 */
	template <typename Enum>
	inline const char* GetEnumName(Enum enum_idx)
	{
		return magic_enum::enum_name(enum_idx).data();
	}
}
