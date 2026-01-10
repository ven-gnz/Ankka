add_library(vma INTERFACE)

target_include_directories(vma
							INTERFACE
							${CMAKE_CURRENT_SOURCE_DIR})

find_package(Vulkan REQUIRED)

target_link_libraries(vma
	INTERFACE
	Vulkan::Vulkan)