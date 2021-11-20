project "Terminator"
	location "Terminator"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	includedirs
	{
		"src",
		"%{wks.location}/Kernel/src",
		"%{wks.location}/Kernel/third_party/spdlog/include",
		"%{wks.location}/Kernel/third_party/debugbreak",
	}

	links
	{
		"Kernel",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			-- "_WINDLL",
		}

	filter "configurations:Debug"
		defines "WUYA_DEBUG"
		runtime "Debug"
		symbols "On"
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribute"
		defines "WUYA_DISTRIBUTE"
		runtime "Release"
		symbols "On"
