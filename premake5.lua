-- Include dirs for projects
IncludeDirs = {}
IncludeDirs["spdlog"] = "%{wks.location}/Kernel/third_party/spdlog/include"
IncludeDirs["debugbreak"] = "%{wks.location}/Kernel/third_party/debugbreak"
IncludeDirs["GLFW"] = "%{wks.location}/Kernel/third_party/glfw/include"
IncludeDirs["Glad"] = "%{wks.location}/Kernel/third_party/glad/include"
IncludeDirs["ImGui"] = "%{wks.location}/Kernel/third_party/imgui"
IncludeDirs["glm"] = "%{wks.location}/Kernel/third_party/glm"
IncludeDirs["stb"] = "%{wks.location}/Kernel/third_party/stb"

-- Solution configuration
workspace "Wuya"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Distribute",
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

group "Kernel/Libraries"
	include "Kernel/third_party"
group ""

group "App"
	include "Editor"
	include "Editor2D"
group ""

group "Samples"
	include "Samples/001_Triangle"
	include "Samples/002_Texture"
	include "Samples/003_Camera"
	include "Samples/004_FrameBuffer"
group ""

