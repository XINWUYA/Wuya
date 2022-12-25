project "Kernel"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	--cppdialect "C++latest"
	cppdialect "C++17"

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
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.debugbreak}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.tinyxml2}",
		"%{IncludeDirs.xxHash}",
		"%{IncludeDirs.tinyobjloader}",
		"%{IncludeDirs.magic_enum}",
		"%{IncludeDirs.VulkanSDK}",
	}
	
	defines
	{
		"PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS",
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"tinyxml2",
		"tinyobjloader",
		"xxHash",
		"opengl32.lib",
		"%{Librarys.shaderc}"
	}
	
	disablewarnings 
	{ 
		"4244", 
		"4267", 
		"4312" 
	}

	filter "system:windows"
		systemversion "latest"

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
		optimize "on"