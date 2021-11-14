workspace "Wuya"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Distribute",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- "Kernel"
project "Kernel"
	location "Kernel"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/third_party/spdlog/include",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		-- systemversion "10.0.19041.0"

		defines
		{
			-- "_WINDLL",
			"WUYA_EXPORT",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Terminator")
		}

	filter "configurations:Debug"
		defines "WUYA_DEBUG"
		symbols "On"
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		optimize "On"

	filter "configurations:Distribute"
		defines "WUYA_DISTRIBUTE"
		symbols "On"

-- "Terminator"
project "Terminator"
	location "Terminator"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",
		"Kernel/src",
		"Kernel/third_party/spdlog/include",
	}

	links
	{
		"Kernel",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		-- systemversion "10.0.19041.0"

		defines
		{
			-- "_WINDLL",
		}

	filter "configurations:Debug"
		defines "WUYA_DEBUG"
		symbols "On"
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		optimize "On"

	filter "configurations:Distribute"
		defines "WUYA_DISTRIBUTE"
		symbols "On"