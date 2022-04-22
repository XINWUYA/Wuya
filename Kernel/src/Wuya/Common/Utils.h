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
		return std::to_string(value.x) + " " + std::to_string(value.y) + " " + std::to_string(value.z) + std::to_string(value.w);
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
}
