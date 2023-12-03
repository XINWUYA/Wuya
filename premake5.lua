-- Dependencies for projects

-- IncludeDirs
VulkanSDKDir = os.getenv("VULKAN_SDK")

IncludeDirs = {}
IncludeDirs["spdlog"] = "%{wks.location}/Libraries/ThirdParty/spdlog/include"
IncludeDirs["debugbreak"] = "%{wks.location}/Libraries/ThirdParty/debugbreak"
IncludeDirs["GLFW"] = "%{wks.location}/Libraries/ThirdParty/glfw/include"
IncludeDirs["Glad"] = "%{wks.location}/Libraries/ThirdParty/glad/include"
IncludeDirs["ImGui"] = "%{wks.location}/Libraries/ThirdParty/imgui"
IncludeDirs["glm"] = "%{wks.location}/Libraries/ThirdParty/glm"
IncludeDirs["stb"] = "%{wks.location}/Libraries/ThirdParty/stb"
IncludeDirs["entt"] = "%{wks.location}/Libraries/ThirdParty/entt/single_include"
IncludeDirs["tinyxml2"] = "%{wks.location}/Libraries/ThirdParty/tinyxml2"
IncludeDirs["ImGuizmo"] = "%{wks.location}/Libraries/ThirdParty/ImGuizmo"
IncludeDirs["xxHash"] = "%{wks.location}/Libraries/ThirdParty/xxHash"
IncludeDirs["tinyobjloader"] = "%{wks.location}/Libraries/ThirdParty/tinyobjloader"
IncludeDirs["magic_enum"] = "%{wks.location}/Libraries/ThirdParty/magic_enum/include"
IncludeDirs["assimp"] = "%{wks.location}/Libraries/ThirdParty/assimp/include"
IncludeDirs["VulkanSDK"] = "%{VulkanSDKDir}/Include"

-- LibDirs
LibraryDirs = {}
LibraryDirs["VulkanSDK"] = "%{VulkanSDKDir}/Lib"

-- Libs
Librarys = {}
Librarys["shaderc"] = "%{LibraryDirs.VulkanSDK}/shaderc_shared.lib"

-- Solution configuration
workspace "Wuya"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Shipping",
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Projects
group "Kernel"
	include "Kernel"
group ""

group "Libraries"
	include "Libraries/ThirdParty"
group ""

group "App"
	include "Editor"
	include "ModelEditor"
group ""

group "Samples"
	include "Samples/Sample_SkyBox"
	include "Samples/Sample_CSM"
--	include "Samples/001_Triangle"
--	include "Samples/002_Texture"
--	include "Samples/003_Camera"
--	include "Samples/004_FrameBuffer"
group ""

