cmake_minimum_required(VERSION 3.20)

# Third-party libraries configuration

# GLFW
project(GLFW C)

add_library(GLFW STATIC)

# Common GLFW sources
set(GLFW_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/GLFW/glfw3.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include/GLFW/glfw3native.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/input.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/monitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/vulkan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/window.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/null_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/null_monitor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/null_joystick.c
)

# Platform-specific sources
if(WIN32)
    set(GLFW_PLATFORM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_joystick.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_monitor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_time.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/win32_module.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/null_window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/egl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/wgl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/osmesa_context.c
    )
    target_compile_definitions(GLFW PRIVATE _GLFW_WIN32)
    target_link_libraries(GLFW PRIVATE Dwmapi)
elseif(APPLE)
    set(GLFW_PLATFORM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/cocoa_init.m
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/cocoa_joystick.m
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/cocoa_monitor.m
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/cocoa_window.m
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/cocoa_time.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/posix_thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/posix_module.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/null_window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/egl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/nsgl_context.m
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/osmesa_context.c
    )
    target_compile_definitions(GLFW PRIVATE _GLFW_COCOA)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo REQUIRED)
    target_link_libraries(GLFW PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_VIDEO_FRAMEWORK}
    )
elseif(UNIX)
    set(GLFW_PLATFORM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/x11_init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/x11_monitor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/x11_window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/xkb_unicode.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/linux_joystick.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/posix_time.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/posix_thread.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/posix_module.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/glx_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/egl_context.c
        ${CMAKE_CURRENT_SOURCE_DIR}/glfw/src/osmesa_context.c
    )
    target_compile_definitions(GLFW PRIVATE _GLFW_X11)
    find_package(X11 REQUIRED)
    target_link_libraries(GLFW PRIVATE X11::X11 X11::Xi X11::Xrandr X11::Xxf86vm X11::Xcursor)
endif()

target_sources(GLFW PRIVATE ${GLFW_COMMON_SOURCES} ${GLFW_PLATFORM_SOURCES})
target_include_directories(GLFW PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
target_compile_definitions(GLFW PRIVATE _CRT_SECURE_NO_WARNINGS)
set_target_properties(GLFW PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# Glad
project(Glad C)

add_library(Glad STATIC)

target_sources(Glad PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/glad/glad.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include/KHR/khrplatform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c
)

target_include_directories(Glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
set_target_properties(Glad PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# ImGui
project(ImGui CXX)

add_library(ImGui STATIC)

set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_rectpack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_textedit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imstb_truetype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp
)

target_sources(ImGui PRIVATE ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set_target_properties(ImGui PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# ImGuizmo
project(ImGuizmo CXX)

add_library(ImGuizmo STATIC)

# Only include core ImGuizmo files, exclude example directories
file(GLOB IMGUIZMO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.h"
)

target_sources(ImGuizmo PRIVATE ${IMGUIZMO_SOURCES})
target_include_directories(ImGuizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo)
target_link_libraries(ImGuizmo PUBLIC ImGui)
set_target_properties(ImGuizmo PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# tinyxml2
project(tinyxml2 CXX)

add_library(tinyxml2 STATIC)

file(GLOB TINYXML2_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2/*.h"
)

target_sources(tinyxml2 PRIVATE ${TINYXML2_SOURCES})
target_include_directories(tinyxml2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
set_target_properties(tinyxml2 PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# xxHash
project(xxHash C)

add_library(xxHash STATIC)

file(GLOB XXHASH_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/xxHash/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/xxHash/*.h"
)

# Exclude x86-specific dispatch file on non-x86 platforms (ARM, etc.)
if(NOT (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i686|i386"))
    list(FILTER XXHASH_SOURCES EXCLUDE REGEX ".*xxh_x86dispatch\\.c$")
endif()

target_sources(xxHash PRIVATE ${XXHASH_SOURCES})
target_include_directories(xxHash PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/xxHash)
set_target_properties(xxHash PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# tinyobjloader
project(tinyobjloader CXX)

add_library(tinyobjloader STATIC)

file(GLOB TINYOBJLOADER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader/*.h"
)

# Exclude loader_example.cc which contains a main() function
list(FILTER TINYOBJLOADER_SOURCES EXCLUDE REGEX ".*loader_example\\.cc$")

target_sources(tinyobjloader PRIVATE ${TINYOBJLOADER_SOURCES})
target_include_directories(tinyobjloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader)
set_target_properties(tinyobjloader PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)

# magic_enum
project(magic_enum CXX)

add_library(magic_enum INTERFACE)

target_include_directories(magic_enum INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/magic_enum/include)

# Set folder for interface libraries
set_target_properties(magic_enum PROPERTIES 
    FOLDER "Libraries/ThirdParty"
)

# assimp
project(assimp CXX)

# Generate assimp configuration files
set(ASSIMP_VERSION_MAJOR 5)
set(ASSIMP_VERSION_MINOR 3)
set(ASSIMP_VERSION_PATCH 1)
set(ASSIMP_PACKAGE_VERSION 0)
set(GIT_COMMIT_HASH "0")
set(GIT_BRANCH "master")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(LIBRARY_SUFFIX "")
set(CMAKE_DEBUG_POSTFIX "")

# Configure assimp config.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include/assimp/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/include/assimp/config.h
    @ONLY
)

# Configure assimp revision.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/assimp/revision.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/revision.h
    @ONLY
)

# Configure zlib zconf.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/zlib/zconf.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/assimp/contrib/zlib/zconf.h
    @ONLY
)

add_library(assimp STATIC)

# Define Z_HAVE_UNISTD_H on Unix/macOS to include unistd.h
# This provides declarations for write(), read(), close() etc.
if(UNIX OR APPLE)
    target_compile_definitions(assimp PRIVATE Z_HAVE_UNISTD_H)
endif()

file(GLOB_RECURSE ASSIMP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/code/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/code/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/pugixml/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/pugixml/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/Open3DGC/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/openddlparser/code/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/unzip/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/zlib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/zlib/*.h"
)

# Exclude zlib example/test programs that are not needed for the library
list(FILTER ASSIMP_SOURCES EXCLUDE REGEX ".*/contrib/zlib/contrib/.*")
list(FILTER ASSIMP_SOURCES EXCLUDE REGEX ".*/contrib/zlib/test.*")
list(FILTER ASSIMP_SOURCES EXCLUDE REGEX ".*/contrib/zlib/examples/.*")

target_sources(assimp PRIVATE ${ASSIMP_SOURCES})

target_include_directories(assimp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/include
        ${CMAKE_CURRENT_BINARY_DIR}/assimp/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/code
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/pugixml/src
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/zlib
        ${CMAKE_CURRENT_BINARY_DIR}/assimp/contrib/zlib
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/rapidjson/include
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/unzip
        ${CMAKE_CURRENT_SOURCE_DIR}/assimp/contrib/openddlparser/include
        ${CMAKE_CURRENT_BINARY_DIR}/assimp
)

target_compile_definitions(assimp PRIVATE
    ASSIMP_BUILD_NO_M3D_IMPORTER
    ASSIMP_BUILD_NO_M3D_EXPORTER
    ASSIMP_BUILD_NO_C4D_IMPORTER
    ASSIMP_BUILD_NO_IFC_IMPORTER
    MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0
    ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1
    RAPIDJSON_HAS_STDSTRING=1
    RAPIDJSON_NOMEMBERITERATORCLASS
    OPENDDLPARSER_BUILD
    _SCL_SECURE_NO_WARNINGS
    _CRT_SECURE_NO_WARNINGS
)

# Fix fdopen redefinition issue on macOS
# zutil.h incorrectly defines fdopen as NULL on Unix systems that do have fdopen
# Solution: Pre-define fdopen as a function-like macro before zutil.h is included
# This makes the #ifndef fdopen check fail, preventing the incorrect NULL definition
if(APPLE)
    target_compile_definitions(assimp PRIVATE "fdopen(fd,mode)=fdopen(fd,mode)")
endif()

set_target_properties(assimp PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    FOLDER "Libraries/ThirdParty"
)
