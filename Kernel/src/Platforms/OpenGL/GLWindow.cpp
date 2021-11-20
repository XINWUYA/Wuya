#include "pch.h"
#include "GLWindow.h"
#include "Wuya/Events/ApplicationEvent.h"
#include "Wuya/Events/KeyEvent.h"
#include "Wuya/Events/MouseEvent.h"

namespace Wuya
{
	static uint8_t s_GLFWWindowCnt = 0;

	GLWindow::GLWindow(const WindowConfig& config)
	{
		Init(config);
	}

	GLWindow::~GLWindow()
	{
		ShutDown();
	}

	void GLWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	bool GLWindow::IsVSync() const
	{
		return m_WindowInfo.IsVSync;
	}

	void GLWindow::SetVSync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);


	}

	void GLWindow::Init(const WindowConfig& config)
	{
		m_WindowInfo.Title = config.Title;
		m_WindowInfo.Width = config.Width;
		m_WindowInfo.Height = config.Height;
		m_WindowInfo.IsVSync = config.IsVSync;

		CORE_LOG_INFO("Create GLFW window {0}: {1}x{2}", config.Title, config.Width, config.Height);

		if (s_GLFWWindowCnt == 0)
		{
			// ��ʼ��GLFW
			int success = glfwInit();

			CORE_ASSERT(success, "Failed to init GLFW!");
			glfwSetErrorCallback([](int error_code, const char* msg)
				{
					CORE_LOG_ERROR("GLFW Error: {0}: {1}", error_code, msg);
				});
		}

#ifdef WUYA_DEBUG
		// todo: ����APIΪOpenGLʱ��������
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		// ����GLFW����
		m_pGLFWWindow = glfwCreateWindow(config.Width, config.Height, config.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_pGLFWWindow);
		++s_GLFWWindowCnt;

		// ָ��������Ϣ
		glfwSetWindowUserPointer(m_pGLFWWindow, &m_WindowInfo);
		SetVSync(config.IsVSync);

		// ���ô���Resize�ص�
		glfwSetWindowSizeCallback(m_pGLFWWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);
				info.Width = width;
				info.Height = height;

				WindowResizeEvent event(width, height);
				info.CallBackFunc(event);
			});

		// ���ô���Close�ص�
		glfwSetWindowCloseCallback(m_pGLFWWindow, [](GLFWwindow* window)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				info.CallBackFunc(event);
			});

		// ���ü��̻ص�
		glfwSetKeyCallback(m_pGLFWWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						info.CallBackFunc(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						info.CallBackFunc(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						info.CallBackFunc(event);
						break;
					}
				}
			});

		// ���������ı��ص�
		glfwSetCharCallback(m_pGLFWWindow, [](GLFWwindow* window, unsigned int key)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(key);
				info.CallBackFunc(event);
			});

		// ������갴���ص�
		glfwSetMouseButtonCallback(m_pGLFWWindow, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						info.CallBackFunc(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						info.CallBackFunc(event);
						break;
					}
				}
			});

		// ���������ֻص�
		glfwSetScrollCallback(m_pGLFWWindow, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				info.CallBackFunc(event);
			});

		// ���������ص�
		glfwSetCursorPosCallback(m_pGLFWWindow, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowInfo& info = *(WindowInfo*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xpos, (float)ypos);
				info.CallBackFunc(event);
			});
	}

	void GLWindow::ShutDown()
	{
		glfwDestroyWindow(m_pGLFWWindow);
		--s_GLFWWindowCnt;

		if (s_GLFWWindowCnt == 0)
			glfwTerminate();
	}
}