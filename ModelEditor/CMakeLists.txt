cmake_minimum_required(VERSION 3.20)

project(ModelEditor LANGUAGES CXX)

# Executable
add_executable(ModelEditor)

# Source files
file(GLOB_RECURSE MODELEDITOR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

target_sources(ModelEditor PRIVATE ${MODELEDITOR_SOURCES})

# Organize source files in Visual Studio by directory structure
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${MODELEDITOR_SOURCES})


# Precompiled header
if(MSVC)
    target_precompile_headers(ModelEditor PRIVATE
        <Pch.h>
    )
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Pch.cpp PROPERTIES COMPILE_FLAGS "/YcPch.h")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_precompile_headers(ModelEditor PRIVATE
        <Pch.h>
    )
endif()

# Include directories
target_include_directories(ModelEditor
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
target_link_libraries(ModelEditor
    PRIVATE
        Kernel
        ImGuizmo
)

# Build configuration specific settings
target_compile_definitions(ModelEditor
    PRIVATE
        $<$<CONFIG:Debug>:HELIOS_DEBUG>
        $<$<CONFIG:Release>:HELIOS_RELEASE>
        $<$<CONFIG:Shipping>:HELIOS_SHIPPING>
)

# Output directory
set_target_properties(ModelEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR}"
    FOLDER "Applications"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(ModelEditor PRIVATE PLATFORM_WINDOWS)
elseif(APPLE)
    set_target_properties(ModelEditor PROPERTIES
        MACOSX_BUNDLE ON
    )
endif()

# Copy assets to output directory (optional)
if(WIN32)
    add_custom_command(TARGET ModelEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:ModelEditor>/Assets"
        COMMENT "Copying Assets to output directory"
    )
endif()
