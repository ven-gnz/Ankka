#include "Ankka/Window.h"
#include <iostream>

bool Window::init(unsigned int width, unsigned int height, std::string title, bool vulkan)
{

	isVulkan = vulkan;

	if (!glfwInit())
	{
		Logger::log(1, "%s: glfwInit() error\n",
			__FUNCTION__);
		return false;
	}

	if (isVulkan)
	{	
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		if (!glfwVulkanSupported())
		{
			glfwTerminate();
			Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
			return false;
		}
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

	if (isVulkan)
	{
		if (!initVulkan())
		{
			return false;
		}
		mVkRenderer = std::make_unique<VkRenderer>();
		
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
	cleanup();
	
}

void Window::cleanup()
{
	if (isVulkan)
	{
		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyInstance(mInstance, nullptr);
	}
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

// not spock pls, with K
bool Window::initVulkan()
{

	VkResult result = VK_ERROR_UNKNOWN;

	VkApplicationInfo mAppInfo{};
	mAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	mAppInfo.pNext = nullptr;
	mAppInfo.pApplicationName = mApplicationName.c_str();
	mAppInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	mAppInfo.pEngineName = "Ankkuli";
	mAppInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	mAppInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);

	uint32_t extensionCount = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	if (extensionCount == 0)
	{
		Logger::log(1, "%s error : no Vulkan extensions found \n", __FUNCTION__);
		return false;
	}

	uint32_t count;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	std::vector<VkExtensionProperties> available(count);
	vkEnumerateInstanceExtensionProperties(nullptr, &count, available.data());

	for (const auto& ext : available)
	{
		std::cout << ext.extensionName << std::endl;
	}

	VkInstanceCreateInfo mCreateInfo{};
	mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.pApplicationInfo = &mAppInfo;
	mCreateInfo.enabledExtensionCount = extensionCount;
	mCreateInfo.ppEnabledExtensionNames = extensions;
	mCreateInfo.enabledLayerCount = 0;

	result = vkCreateInstance(&mCreateInfo, nullptr, &mInstance);
	if (result != VK_SUCCESS)
	{
		Logger::log(1, "%s: Could not create instance (%i)\n", __FUNCTION__, result);
		return false;
	}

	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0)
	{
		Logger::log(1, "%s: No vulkan capable GPU found\n", __FUNCTION__);
		return false;
	}

	std::vector<VkPhysicalDevice> devices;
	vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, devices.data());

	result = glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface);
	if (result != VK_SUCCESS)
	{
		Logger::log(1, "%s: Could not create Vulkan surface\n", __FUNCTION__);

		return false;
	}
}