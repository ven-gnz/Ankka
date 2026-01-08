#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include <Ankka/Logger.h>
#include "opengl/OGLRenderer.h"
#include "Model.h"

class Window {
public:
	bool init(unsigned int width, unsigned int height, std::string title, bool vulkan);
	bool initVulkan();
	void mainLoop();
	void cleanup();
	void handleKeyEvents(int key, int scancode, int action, int mods);

private:
	
	void handleWindowCloseEvents();
	void handleMouseButtonEvents(int button, int action, int mods);
	GLFWwindow* mWindow = nullptr;
	std::string mApplicationName;
	VkInstance mInstance{};
	VkSurfaceKHR mSurface{};
	bool editTitle = false;
	std::string title;
	std::string newTitle;
	std::unique_ptr<OGLRenderer> mRenderer;
	std::unique_ptr<Model> mModel;
	bool isVulkan;
};