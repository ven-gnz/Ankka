#include "Ankka/Window.h"
#include <iostream>

void Window::handleWindowCloseEvents()
{
	
	Logger::log(1, "%s : Window close event fired \n", __FUNCTION__);
	cleanup();
	
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods)
{
	std::string actionName;
	switch (action)
	{
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	case GLFW_REPEAT:
		actionName = "repeated";
		break;
	default:
		actionName = "invalid";
		break;
	}
	const char* keyName = glfwGetKeyName(key, 0);

	if (!editTitle)
	{
		
		Logger::log(1, "%s : key %s (key %i, scancode %i) %s\n", __FUNCTION__,
		keyName, key, scancode, actionName.c_str());
	}

	else
	{
		if (editTitle && keyName != nullptr && action == GLFW_PRESS)
		{
			newTitle.append(keyName);
		}
	}
	
if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
{
	editTitle = !editTitle;
	if (!editTitle)
	{
		title = newTitle;
		glfwSetWindowTitle(mWindow, title.c_str());
		Logger::log(1, "New title of app : %s\n", title.c_str());
		newTitle.clear();
	}
	
	return;
	}

}

void Window::handleMouseButtonEvents(int button, int action, int mods)
{
	std::string actionName;
	switch (action)
	{
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	default:
		actionName = "invalid";
		break;
	}
	std::string mouseButtonName;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseButtonName = "left";
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mouseButtonName = "middle";
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseButtonName = "right";
		break;
	}

	Logger::log(1, "%s: %s mouse button (%i) %s\n", __FUNCTION__, mouseButtonName.c_str(), button, actionName.c_str());

}

bool Window::init(unsigned int width, unsigned int height, std::string title_initial)
{
	title = title_initial;
	newTitle = "";

	if (!glfwInit())
	{
		Logger::log(1, "%s: glfwInit() error\n",
			__FUNCTION__);
		return false;
	}

	//if (!glfwVulkanSupported())
	//{
	//	glfwTerminate();
	//	Logger::log(1, "%s: Vulkan is not supported\n", __FUNCTION__);
	//	return false;
	//}

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	mApplicationName = title;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	mWindow = glfwCreateWindow(width, height, mApplicationName.c_str(), nullptr, nullptr);
	

	if (!mWindow)
	{
		Logger::log(1, "%s: could not create window\n",
			__FUNCTION__);

		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(mWindow);

	//if (!initVulkan())
	//{
	//	Logger::log(1, "%s : could not init Vulkan\n", __FUNCTION__);

	//	glfwTerminate();
	//	return false;
	//}


	mRenderer = std::make_unique<OGLRenderer>();
	if (!mRenderer->init(width, height))
	{
		glfwTerminate();
		return false;
	}

	glfwSetWindowUserPointer(mWindow, mRenderer.get());





	//glfwSetKeyCallback(mWindow, [](GLFWwindow* win, int key, int scancode, int action, int mods)
	//	{
	//		auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(win));
	//		thisWindow->handleKeyEvents(key, scancode, action, mods);
	//	});

	//glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* win)
	//	{
	//		auto thisWindow = static_cast<Window*>(
	//			glfwGetWindowUserPointer(win));
	//		thisWindow->handleWindowCloseEvents();
	//	});

	//glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* win, int button, int action, int mods)
	//	{
	//		auto thisWindow = static_cast<Window*>(
	//			glfwGetWindowUserPointer(win));
	//		thisWindow->handleMouseButtonEvents(button, action, mods);

	//	});

	glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* win, int width, int height)
		{
			auto renderer = static_cast<OGLRenderer*>(glfwGetWindowUserPointer(win));
				renderer->setSize(width, height);
				
		});

	mModel = std::make_unique<Model>();
	mModel->init();

	
	Logger::log(1, "%s: Window succesfully initialized\n",
		__FUNCTION__);
	return true;
}


void Window::mainLoop()
{
	glfwSwapInterval(1);
	float color = 0.0f;
	while (!glfwWindowShouldClose(mWindow)) {


		mRenderer->draw();
		glfwSwapBuffers(mWindow);
		glfwPollEvents();

		
	}
	
}

void Window::cleanup()
{
	//vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	//vkDestroyInstance(mInstance, nullptr);
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