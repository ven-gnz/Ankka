#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

#include <Ankka/Logger.h>

class Window {
public:
	bool init(unsigned int width, unsigned int height, std::string title);
	bool initVulkan();
	void mainLoop();
	void cleanup();
	void handleKeyEvents(int key, int scancode, int action, int mods);

private:
	void handleWindowCloseEvents();
	void handleMouseButtonEvents(int butotn, int action, int mods);
	GLFWwindow* mWindow = nullptr;
	std::string mApplicationName;
	VkInstance mInstance{};
	VkSurfaceKHR mSurface{};
};