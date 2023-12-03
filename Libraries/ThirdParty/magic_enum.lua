project "magic_enum"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	--cppdialect "C++latest"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"magic_enum/include/magic_enum.hpp",
		"magic_enum/include/magic_enum_format.hpp",
		"magic_enum/include/magic_enum_fuse.hpp",
		"magic_enum/include/magic_enum_switch.hpp",
		"magic_enum/example/example.cpp",
		"magic_enum/example/enum_flag_example.cpp",
		"magic_enum/example/example_custom_name.cpp",
		"magic_enum/example/example_switch.cpp",
	}
	
	includedirs
	{
		"magic_enum/include",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"