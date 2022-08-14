project "xxHash"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	--cppdialect "C++latest"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"xxHash/xxhash.h",
		"xxHash/xxhash.c",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"