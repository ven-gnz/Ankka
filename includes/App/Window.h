#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>

#include <tools/Logger.h>
#include "opengl/OGLRenderer.h"


class Window {
public:
	bool init(unsigned int width, unsigned int height, std::string title, bool vulkan);
	void mainLoop();
	void cleanup();

private:
	
	GLFWwindow* mWindow = nullptr;
	std::string mApplicationName;
	std::unique_ptr<OGLRenderer> mOGLRenderer;
	bool isVulkan;
};