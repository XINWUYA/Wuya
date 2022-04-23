project "ImGuizmo"
	kind "StaticLib"
	staticruntime "on"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/build/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"ImGuizmo/ImGuizmo.h",
		"ImGuizmo/ImGuizmo.cpp",
		"ImGuizmo/GraphEditor.h",
		"ImGuizmo/GraphEditor.cpp",
		"ImGuizmo/ImCurveEdit.h",
		"ImGuizmo/ImCurveEdit.cpp",
		"ImGuizmo/ImGradient.h",
		"ImGuizmo/ImGradient.cpp",
		"ImGuizmo/ImSequencer.h",
		"ImGuizmo/ImSequencer.cpp",
		"ImGuizmo/ImZoomSlider.h",
	}
	
	includedirs
	{
		"%{IncludeDirs.ImGui}",
	}
	
	links
	{
		"ImGui",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"