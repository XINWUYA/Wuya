#include "Pch.h"
#include "Utils.h"
#include "xxhash.h"

namespace Wuya
{
	/* Extract file base dir from path
	 * Example:
	 * path: "assets/scenes/test.scn"
	 * return: "assets/scenes/"
	 */
	std::string ExtractFileBaseDir(const std::string& path)
	{
		auto last_slash = path.find_last_of("/\\");
		last_slash = (last_slash == std::string::npos) ? 0 : last_slash + 1;
		return path.substr(0, last_slash);
	}

	/* Extract file base name from path
	 * Example:
	 * path: "assets/scenes/test.scn"
	 * return: "test.scn"
	 */
	std::string ExtractFileBaseName(const std::string& path)
	{
		const auto last_slash = path.find_last_of("/\\");
		return path.substr(last_slash + 1);
	}

	/* Extract file base dir & base name from path
	 * Example:
	 * path: "assets/scenes/test.scn"
	 * return: {"assets/scenes/", "test.scn"}
	 */
	std::pair<std::string, std::string> ExtractFileBaseDirAndBaseName(const std::string& path)
	{
		auto last_slash = path.find_last_of("/\\");
		last_slash = (last_slash == std::string::npos) ? 0 : last_slash + 1;
		return { path.substr(0, last_slash), path.substr(last_slash) };
	}

	/* Extract filename from path
	 * Examples:
	 * path: "assets/scenes/test.scn"
	 * return: "test"
	 */
	std::string ExtractFilename(const std::string& path)
	{
		auto last_slash = path.find_last_of("/\\");
		last_slash = (last_slash == std::string::npos) ? 0 : last_slash + 1;
		const auto last_dot = path.rfind('.');
		const auto count = (last_dot == std::string::npos) ? path.size() - last_slash : last_dot - last_slash;
		return path.substr(last_slash, count);
	}

	/* Extract filename from path
	 * Examples:
	 * path: "assets/scenes/test.scn"
	 * return: "scn"
	 */
	std::string ExtractFileSuffix(const std::string& path)
	{
		const auto last_dot = path.rfind('.');
		return path.substr(last_dot + 1);
	}

	/* Replace file suffix
	 * Examples:
	 * path: "assets/scenes/test.scn", suffix: "bin"
	 * return: "assets/scenes/test.bin"
	 */
	std::string ReplaceFileSuffix(const std::string& path, const std::string& suffix)
	{
		const auto last_dot = path.rfind('.');
		return path.substr(0, last_dot) + suffix;
	}

	/* string to Id
	 * use xxHash
	 */
	uint32_t ToID(const std::string& value)
	{
		return XXH32(value.c_str(), value.size(), 0);
	}
}
