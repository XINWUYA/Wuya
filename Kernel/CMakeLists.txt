cmake_minimum_required(VERSION 3.20)

project(Kernel LANGUAGES CXX)

# Static library
add_library(Kernel STATIC)

# Source files
file(GLOB_RECURSE KERNEL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# Precompiled header
set(PCH_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/Pch.h")
set(PCH_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/Pch.cpp")

# Copy TracyClient.cpp before build and insert Pch.h
set(TRACY_CLIENT_SRC "${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tracy/public/TracyClient.cpp")
set(TRACY_CLIENT_DST "${CMAKE_CURRENT_SOURCE_DIR}/src/Wuya/Tracy/TracyClient.cpp")

add_custom_command(
    OUTPUT ${TRACY_CLIENT_DST}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TRACY_CLIENT_SRC} ${TRACY_CLIENT_DST}
    COMMENT "Copying TracyClient.cpp"
)

# Add sources
target_sources(Kernel PRIVATE
    ${KERNEL_SOURCES}
    ${TRACY_CLIENT_DST}
)

# Precompiled header
if(MSVC)
    target_precompile_headers(Kernel PRIVATE
        <Pch.h>
    )
    set_source_files_properties(${PCH_SOURCE} PROPERTIES COMPILE_FLAGS "/YcPch.h")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_precompile_headers(Kernel PRIVATE
        <Pch.h>
    )
endif()

# Include directories
target_include_directories(Kernel
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}  # For WuyaConfig.h
    PRIVATE
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/spdlog/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/debugbreak
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glfw/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glad/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/imgui
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glm
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/stb
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/entt/single_include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tinyxml2
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/xxHash
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tinyobjloader
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/magic_enum/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/assimp/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tracy/public
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/metal-cpp  # Metal C++ wrapper
)

if(VULKAN_SDK)
    target_include_directories(Kernel PRIVATE ${VULKAN_SDK}/Include)
endif()

# Compile definitions
target_compile_definitions(Kernel
    PRIVATE
        GLFW_INCLUDE_NONE
        TRACY_ENABLE
)

# Link libraries
target_link_libraries(Kernel
    PUBLIC
        GLFW
        Glad
        ImGui
        tinyxml2
        tinyobjloader
        assimp
        xxHash
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(Kernel PRIVATE opengl32)
    if(VULKAN_SDK)
        target_link_libraries(Kernel PRIVATE ${VULKAN_SDK}/Lib/shaderc_shared.lib)
    endif()
elseif(APPLE)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(METAL_FRAMEWORK Metal REQUIRED)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
    target_link_libraries(Kernel PRIVATE ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK} ${METAL_FRAMEWORK} ${QUARTZCORE_FRAMEWORK})
elseif(UNIX)
    target_link_libraries(Kernel PRIVATE OpenGL::GL)
endif()

# Build configuration specific settings
target_compile_definitions(Kernel
    PRIVATE
        $<$<CONFIG:Debug>:WUYA_DEBUG>
        $<$<CONFIG:Release>:WUYA_RELEASE>
        $<$<CONFIG:Shipping>:WUYA_SHIPPING>
)

# Output directory
set_target_properties(Kernel PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${OUTPUT_DIR}"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Kernel PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(Kernel PRIVATE PLATFORM_MACOS)
    # Compile FileDialog.cpp as Objective-C++ on macOS using compiler flags
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Wuya/Application/FileDialog.cpp"
        PROPERTIES 
            COMPILE_FLAGS "-x objective-c++"
            SKIP_PRECOMPILE_HEADERS ON
    )
    # Compile ImGuiLayer.cpp as Objective-C++ on macOS for Metal support
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Wuya/ImGui/ImGuiLayer.cpp"
        PROPERTIES 
            COMPILE_FLAGS "-x objective-c++"
            SKIP_PRECOMPILE_HEADERS ON
    )
    # Compile MetalQuery.cpp as Objective-C++ on macOS
    set_source_files_properties(
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GraphicsAPI/Metal/MetalQuery.cpp"
        PROPERTIES 
            COMPILE_FLAGS "-x objective-c++"
            SKIP_PRECOMPILE_HEADERS ON
    )
elseif(UNIX)
    target_compile_definitions(Kernel PRIVATE PLATFORM_LINUX)
endif()
