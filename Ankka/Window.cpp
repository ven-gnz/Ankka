#include "Ankka/Window.h"
#include <iostream>

bool Window::init(unsigned int width, unsigned int height, std::string title, bool vulkan)
{


	if (!glfwInit())
	{
		Logger::log(1, "%s: glfwInit() error\n",
			__FUNCTION__);
		return false;
	}

	else
	{
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		mApplicationName = title;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}



	mWindow = glfwCreateWindow(width, height, mApplicationName.c_str(), nullptr, nullptr);
	

	if (!mWindow)
	{
		Logger::log(1, "%s: could not create window\n",
			__FUNCTION__);
		glfwTerminate();
		return false;
	}
	if (!isVulkan)
	{
		glfwMakeContextCurrent(mWindow);
		mOGLRenderer = std::make_unique<OGLRenderer>(mWindow);
		if (!mOGLRenderer->init(width, height))
		{
			glfwTerminate();
			return false;
		}
		glfwSetWindowUserPointer(mWindow, mOGLRenderer.get());
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* win, int width, int height)
			{
				auto renderer = static_cast<OGLRenderer*>(glfwGetWindowUserPointer(win));
				renderer->setSize(width, height);
			});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods)
			{
				auto renderer = static_cast<OGLRenderer*>(glfwGetWindowUserPointer(win));
				renderer->handleKeyEvents(key, scancode, action, mods);
			});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods)
			{
				auto renderer = static_cast<OGLRenderer*>
					(glfwGetWindowUserPointer(win));
				renderer->handleMouseButtonEvents(button, action, mods);
			});
		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* win, double xpos, double ypos)
			{
				auto renderer = static_cast<OGLRenderer*>
					(glfwGetWindowUserPointer(win));
				renderer->handleMousePositionEvents(xpos, ypos);
			});

		mModel = std::make_unique<Model>();
		mModel->init();

		Logger::log(1, "%s: mockup model data loaded\n", __FUNCTION__);
		Logger::log(1, "%s: Window succesfully initialized\n",
			__FUNCTION__);
		return true;
	}
	

}


void Window::mainLoop()
{
	glfwSwapInterval(1);
	float color = 0.0f;
	if(!isVulkan) mOGLRenderer->uploadData(mModel->getVertexData());
	while (!glfwWindowShouldClose(mWindow)) {

		mOGLRenderer->draw();
		glfwSwapBuffers(mWindow);
	
		glfwPollEvents();
		
	}
	
}

void Window::cleanup()
{

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

