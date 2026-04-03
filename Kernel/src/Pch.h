#pragma once

/* 确保Windows Visual Studio正确处理UTF-8编码的源文件 */
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "WuyaConfig.h"

#include <cstdint>
#include <string>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "Wuya/Common/Utils.h"
#include "Wuya/Common/Common.h"
#include "Wuya/Common/Assert.h"
#include "Wuya/Core/Logger.h"

#include "Wuya/Core/Profiler.h"