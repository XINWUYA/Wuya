#include "Pch.h"
#include "Utils.h"
#include "xxhash.h"

namespace Wuya
{

	uint32_t ToID(const std::string& value)
	{
		return XXH32(value.c_str(), value.size(), 0);
	}
}
