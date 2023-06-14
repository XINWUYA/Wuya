project "ModelEditor"
	kind "ConsoleApp"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"
	--cppdialect "C++latest"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "Pch.h"
	pchsource "src/Pch.cpp"
	
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
		"%{IncludeDirs.ImGuizmo}",
		"%{IncludeDirs.tinyobjloader}",
		"%{IncludeDirs.magic_enum}",
		"%{IncludeDirs.assimp}",
	}

	links
	{
		"Kernel",
		"ImGuizmo",
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

	filter "configurations:Shipping"
		defines "WUYA_SHIPPING"
		runtime "Release"
		symbols "on"
