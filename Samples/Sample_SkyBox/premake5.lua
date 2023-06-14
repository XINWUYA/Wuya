project "Sample_SkyBox"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

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
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.tinyxml2}",
		"%{IncludeDirs.magic_enum}",
	}

	links
	{
		"Kernel",
	}
	
	disablewarnings 
	{ 
		"4244", 
		"4312",
		"4819",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS",
			"_CRT_SECURE_NO_WARNINGS",
			"ASSETS_PATH=R\"($(SolutionDir)Assets)\""
		}

	filter "configurations:Debug"
		defines "WUYA_DEBUG"
		runtime "Debug"
		symbols "on"
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		runtime "Release"
		optimize "on"