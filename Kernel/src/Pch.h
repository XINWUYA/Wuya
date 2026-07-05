#pragma once

/* Ensure Windows Visual Studio correctly handles UTF-8 encoded source files */
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

/* === Standard Library (stable, frequently used) === */
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <functional>
#include <utility>
#include <optional>
#include <tuple>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

/* === Third-party libraries (stable, large headers) === */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <entt/entt.hpp>

/* === Engine Common (included by almost every module) === */
#include "HeliosConfig.h"
#include "Helios/Common/Utils.h"
#include "Helios/Common/Common.h"
#include "Helios/Common/Assert.h"
#include "Helios/Core/Logger.h"
#include "Helios/Core/Profiler.h"