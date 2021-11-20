-- Include dirs for projects
IncludeDirs = {}
IncludeDirs["GLFW"] = "%{wks.location}/Kernel/third_party/glfw/include"

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
include "Kernel"
include "Terminator"

group "Libraries"
	include "Kernel/third_party"
group ""