cmake_minimum_required(VERSION 3.20)

project(Editor LANGUAGES CXX)

# Executable
add_executable(Editor)

# Source files
file(GLOB_RECURSE EDITOR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(Editor PRIVATE ${EDITOR_SOURCES})

# Precompiled header
if(MSVC)
    target_precompile_headers(Editor PRIVATE
        <Pch.h>
    )
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_precompile_headers(Editor PRIVATE
        <Pch.h>
    )
endif()

# Include directories
target_include_directories(Editor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/Kernel/src
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/spdlog/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/debugbreak
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glfw/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glad/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/imgui
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/glm
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/entt/single_include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tinyxml2
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/ImGuizmo
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/tinyobjloader
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/magic_enum/include
        ${CMAKE_SOURCE_DIR}/Libraries/ThirdParty/assimp/include
)

# Link libraries
target_link_libraries(Editor
    PRIVATE
        Kernel
        ImGuizmo
)

# Build configuration specific settings
target_compile_definitions(Editor
    PRIVATE
        $<$<CONFIG:Debug>:WUYA_DEBUG>
        $<$<CONFIG:Release>:WUYA_RELEASE>
        $<$<CONFIG:Shipping>:WUYA_SHIPPING>
)

# Output directory
set_target_properties(Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR}"
    FOLDER "Applications"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Editor PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    set_target_properties(Editor PROPERTIES
        MACOSX_BUNDLE ON
    )
endif()

# Copy assets to output directory (optional)
if(WIN32)
    add_custom_command(TARGET Editor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:Editor>/Assets"
        COMMENT "Copying Assets to output directory"
    )
endif()
