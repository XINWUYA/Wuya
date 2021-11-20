project "GLFW"
	kind "StaticLib"
	language "C"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"glfw/include/GLFW/glfw3.h",
		"glfw/include/GLFW/glfw3native.h",
		"glfw/src/context.c",
		"glfw/src/init.c",
		"glfw/src/input.c",
		"glfw/src/monitor.c",
		"glfw/src/vulkan.c",
		"glfw/src/window.c",
		"glfw/src/platform.c",
		"glfw/src/null_init.c",
		"glfw/src/null_monitor.c",
		"glfw/src/null_joystick.c",
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		files
		{
			"glfw/src/win32_init.c",
			"glfw/src/win32_joystick.c",
			"glfw/src/win32_monitor.c",
			"glfw/src/win32_thread.c",
			"glfw/src/win32_time.c",
			"glfw/src/win32_window.c",
			"glfw/src/win32_module.c",
			"glfw/src/null_window.c",
			"glfw/src/egl_context.c",
			"glfw/src/wgl_context.c",
			"glfw/src/osmesa_context.c",
		}

		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

		links
		{
			"Dwmapi.lib"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"