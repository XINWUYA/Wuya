project "assimp"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	--cppdialect "C++latest"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")
	
	includedirs
	{
		"assimp",
		"assimp/include",
		"assimp/code",
		"assimp/contrib",
		"assimp/contrib/pugixml/src",
		"assimp/contrib/zlib",
		"assimp/contrib/rapidjson/include",
		"assimp/contrib/unzip",
		"assimp/contrib/openddlparser/include",
	}
	
	files
	{
		"assimp/include/**",
		"assimp/code/**",
		"assimp/contrib/pugixml/src/*",
		"assimp/contrib/Open3DGC/*",
		"assimp/contrib/openddlparser/include/**",
		"assimp/contrib/openddlparser/code/**",
		"assimp/contrib/unzip/*",
		"assimp/contrib/zlib/*.h",
		"assimp/contrib/zlib/*.c",
	}
	
	defines
	{
		"ASSIMP_BUILD_NO_M3D_IMPORTER",
		"ASSIMP_BUILD_NO_M3D_EXPORTER",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
		"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
		"RAPIDJSON_HAS_STDSTRING=1",
		"RAPIDJSON_NOMEMBERITERATORCLASS",
		"OPENDDLPARSER_BUILD",
		"_SCL_SECURE_NO_WARNINGS",
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"