#pragma once

namespace Wuya
{
#define COMPONENT_CLASS(type)													\
	void SetDebugName(const std::string& name) { m_DebugName = #type + name; }	\
	const std::string& GetDebugName() const { return m_DebugName; }

const std::filesystem::path g_AssetsPath = ASSETS_PATH;
#define RELATIVE_PATH(path) (g_AssetsPath / (path)).generic_string()

}
