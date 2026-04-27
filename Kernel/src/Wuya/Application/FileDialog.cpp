#include "Pch.h"
#include "FileDialog.h"
#include "Application.h"
#include "GLFW/glfw3.h"

#ifdef PLATFORM_WINDOWS
#include "commdlg.h"
#include "shellapi.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#elif defined(PLATFORM_MACOS)
#import <Cocoa/Cocoa.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#endif

namespace Wuya
{
	/* 通过文件对话框选取指定的文件路径 */
	std::string FileDialog::OpenFile(const char* filter)	{
#ifdef PLATFORM_WINDOWS
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Instance()->GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
#elif defined(PLATFORM_MACOS)
		@autoreleasepool {
			NSOpenPanel* openPanel = [NSOpenPanel openPanel];
			[openPanel setAllowsMultipleSelection:NO];
			[openPanel setCanChooseDirectories:NO];
			[openPanel setCanChooseFiles:YES];
			
			// Set parent window
			NSWindow* nsWindow = glfwGetCocoaWindow((GLFWwindow*)Application::Instance()->GetWindow().GetNativeWindow());
			[openPanel beginSheetModalForWindow:nsWindow completionHandler:nil];
			
			if ([openPanel runModal] == NSModalResponseOK) {
				NSURL* url = [[openPanel URLs] objectAtIndex:0];
				NSString* path = [url path];
				return std::string([path UTF8String]);
			}
		}
		return std::string();
#else
		return std::string();
#endif
	}

	/* 保存文件到指定路径 */
	std::string FileDialog::SaveFile(const char* filter)	{
#ifdef PLATFORM_WINDOWS
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Instance()->GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
#elif defined(PLATFORM_MACOS)
		@autoreleasepool {
			NSSavePanel* savePanel = [NSSavePanel savePanel];
			[savePanel setCanCreateDirectories:YES];
			
			// Set parent window
			NSWindow* nsWindow = glfwGetCocoaWindow((GLFWwindow*)Application::Instance()->GetWindow().GetNativeWindow());
			[savePanel beginSheetModalForWindow:nsWindow completionHandler:nil];
			
			if ([savePanel runModal] == NSModalResponseOK) {
				NSURL* url = [savePanel URL];
				NSString* path = [url path];
				return std::string([path UTF8String]);
			}
		}
		return std::string();
#else
		return std::string();
#endif
	}

	/* 打开到指定的文件目录 */
	bool OpenFileExplorer(const char* path)
	{
		if (!path || path == "")
			return false;

#ifdef PLATFORM_WINDOWS
		auto select_params = " /select, " + std::string(path);
		std::wstring select_params_ws;
		select_params_ws.assign(select_params.begin(), select_params.end());

		SHELLEXECUTEINFO shex = { 0 };
		shex.cbSize = sizeof(SHELLEXECUTEINFO);
		shex.lpFile = static_cast<LPCSTR>("explorer");
		shex.lpParameters = reinterpret_cast<LPCSTR>(select_params_ws.c_str());
		shex.lpVerb = static_cast<LPCSTR>("open");
		shex.nShow = SW_SHOWDEFAULT;
		shex.lpDirectory = NULL;

		return ShellExecuteEx(&shex);
#elif defined(PLATFORM_MACOS)
		@autoreleasepool {
			NSString* nsPath = [NSString stringWithUTF8String:path];
			NSURL* url = [NSURL fileURLWithPath:nsPath];
			
			// Open Finder and select the file
			NSArray* fileURLs = [NSArray arrayWithObject:url];
			[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:fileURLs];
		}
		return true;
#else
		return false;
#endif
	}
}