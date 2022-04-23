project "Editor"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{wks.location}/Kernel/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.debugbreak}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.tinyxml2}",
	}

	links
	{
		"Kernel",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "WUYA_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribute"
		defines "WUYA_DISTRIBUTE"
		runtime "Release"
		symbols "on"
