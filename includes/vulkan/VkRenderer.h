#pragma once

// Only a stub for now, I made the decision to focus on getting to the meat faster by deferring the development of this.

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

class VkRenderer
{

public:
	bool draw();
};