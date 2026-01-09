add_library(vk-bootstrap
			VkBootstrap.cpp
			VkBootstrap.h
			VkBootstrapDispatch.h)

target_include_directories(vk-bootstrap
							PUBLIC
								${CMAKE_CURRENT_SOURCE_DIR})

find_package(Vulkan REQUIRED)

target_link_libraries(vk-bootstrap
		PUBLIC
				Vulkan::Vulkan)
