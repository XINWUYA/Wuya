#pragma once
#include <iomanip>
#include <string>
#include <sstream>

inline std::string ConvertFloat2String(float value, int precision = -1)
{
	if (precision < 0)
		return std::to_string(value);

	std::ostringstream oss;
	oss.precision(precision);
	oss << std::fixed << value;
	return oss.str();
}
