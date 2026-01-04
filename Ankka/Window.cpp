#include "Ankka/Window.h"

bool Window::init(unsigned int width, unsigned int height, std::string title)
{
	if (!glfwInit())
	{
		Logger::log(1, "%s: glfwInit() error\n",
			__FUNCTION__);
		return false;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!mWindow)
	{
		Logger::log(1, "%s: could not create window\n",
			__FUNCTION__);
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(mWindow);


	Logger::log(1, "%s: Window succesfully initialized\n",
		__FUNCTION__);
	return true;
}


void Window::mainLoop()
{
	glfwSwapInterval(1);
	float color = 0.0f;
	while (!glfwWindowShouldClose(mWindow)) {

		color >= 1 ? color = 0.0f : color += 0.01f;
		glClearColor(color, color, color, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(mWindow);

		glfwPollEvents();
	}
}

void Window::cleanup()
{
	
	glfwDestroyWindow(mWindow);
	glfwTerminate();

}