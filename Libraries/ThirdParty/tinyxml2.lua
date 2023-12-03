project "tinyxml2"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	--cppdialect "C++latest"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"tinyxml2/tinyxml2.h",
		"tinyxml2/tinyxml2.cpp",
		"tinyxml2/xmltest.cpp",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"