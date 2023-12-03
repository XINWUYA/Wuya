-- Copy File
local function CopyFile(srcFilePath, dstFilePath)
	absSrcPath = path.getabsolute(srcFilePath)
	absDstPath = path.getabsolute(dstFilePath)
	absDstDir = path.getdirectory(absDstPath)
    os.mkdir(absDstDir)
    succ, errInfo = os.copyfile(absSrcPath, absDstPath)
	if not succ then
		error(errInfo)
	end
end

-- Insert Pch.h
local function InsertHeader(filePath, lineNum, headerInfo)
	local lines = {}

	-- Read into lines
	local inputFile = io.open(filePath, "r")
	if inputFile then
		for line in inputFile:lines() do
			table.insert(lines, line)
		end
        inputFile:close()
    end

	-- Insert content to target line
	if #lines > 0 then
		table.insert(lines, lineNum, headerInfo)
	end

	-- Write to file
	local outputFile = io.open(filePath, "w")
    if outputFile then
        outputFile:write(table.concat(lines, "\n"))
        outputFile:close()
    end
end

project "Kernel"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	cppdialect "C++latest"
	-- cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "Pch.h"
	pchsource "src/Pch.cpp"

	prebuildcommands
	{
		CopyFile("../Libraries/ThirdParty/tracy/public/TracyClient.cpp", "src/Wuya/Tracy/TracyClient.cpp"),
		InsertHeader("src/Wuya/Tracy/TracyClient.cpp", 1, "#include \"Pch.h\"")
	}

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
		"%{IncludeDirs.assimp}",
		"%{IncludeDirs.tracy}",
		"%{IncludeDirs.VulkanSDK}",
	}
	
	defines
	{
		"PLATFORM_WINDOWS",
		"GLFW_INCLUDE_NONE",
		"_CRT_SECURE_NO_WARNINGS",
		"TRACY_ENABLE",
		"ASSETS_PATH=R\"($(SolutionDir)Assets)\""
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"tinyxml2",
		"tinyobjloader",
		"assimp",
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
		symbols "On"
		buildoptions {'/Zi'}
	
	filter "configurations:Release"
		defines "WUYA_RELEASE"
		runtime "Release"
		optimize "On"
		buildoptions {'/Zi'}

	filter "configurations:Shipping"
		defines "WUYA_SHIPPING"
		runtime "Release"
		optimize "on"