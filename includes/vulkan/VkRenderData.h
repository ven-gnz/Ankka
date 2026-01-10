#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

struct VkRenderData {
	VmaAllocator rdAllocator;
	vkb::Instance rdVkbInstance{};
	vkb::Device rdVkbDevice{};
	vkb::Swapchain rdVkbSwapchain{};

std::vector<VkImage> rdSwapchainImages;
std::vector<VkImageView> rdSwapchainImageViews;
std::vector<VkFramebuffer> rdFramebuffers;
};