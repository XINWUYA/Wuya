-- Include dirs for projects
IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/Kernel/third_party/glfw/include"
IncludeDirs["Glad"] = "%{wks.location}/Kernel/third_party/glad/include"
IncludeDirs["ImGui"] = "%{wks.location}/Kernel/third_party/imgui"
IncludeDirs["glm"] = "%{wks.location}/Kernel/third_party/glm"

-- Solution configuration
workspace "Wuya"
	architecture "x64"
	startproject "Terminator"

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
	include "Terminator"
group ""

group "Samples"
	include "Samples/001_Triangle"
	include "Samples/002_Texture"
	include "Samples/003_Camera"
group ""

