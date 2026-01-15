# CMakeList.txt : CMake project for Ankka, include source and define
# project specific logic here.



# Add source to this project's executable.
add_executable (Ankka
	"Ankka.cpp"
	"../includes/Ankka/Ankka.h"
	"../includes/Ankka/Logger.h"
	"../includes/Ankka/Model.h"
	"../includes/Ankka/UserInterface.h"
	"../includes/Ankka/Window.h"
	"Window.cpp"
	"Logger.cpp"
	"Model.cpp" 
	"UserInterface.cpp"
	"../includes/opengl/OGLRenderer.h"
	"../includes/opengl/Framebuffer.h"
	"../includes/opengl/VertexBuffer.h"
	"../includes/opengl/Texture.h"
	"../includes/opengl/Shader.h" 
	"../includes/opengl/OGLRenderData.h" 
	"opengl/OGLRenderer.cpp" 
	"opengl/Framebuffer.cpp" 
	"opengl/VertexBuffer.cpp" 
	"opengl/Texture.cpp" 
	"opengl/Shader.cpp" 
	"opengl/UniformBuffer.cpp"
	
	"../includes/vulkan/VkRenderer.h" 
	"vulkan/VkRenderer.cpp" )

# Resource management

file(GLOB SHADERS ${CMAKE_CURRENT_BINARY_DIR}/shaders/*)

add_custom_target(refresh_shaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_CURRENT_SOURCE_DIR}/shaders
		${CMAKE_CURRENT_BINARY_DIR}/shaders
	DEPENDS ${SHADERS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tex DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_dependencies(Ankka refresh_shaders)

# Source codde

target_include_directories(Ankka
	PRIVATE 
		${CMAKE_SOURCE_DIR}/includes
		${CMAKE_SOURCE_DIR}/external/imgui)

find_package(Vulkan REQUIRED)

target_link_libraries(Ankka 
	PRIVATE 
	glfw
	glad
	vk-bootstrap
	Vulkan::Vulkan
	imgui)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Ankka PROPERTY CXX_STANDARD 20)
endif()


