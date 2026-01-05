# CMakeList.txt : CMake project for Ankka, include source and define
# project specific logic here.



# Add source to this project's executable.
add_executable (Ankka
	"Ankka.cpp"
	"../includes/Ankka/Ankka.h"
	"../includes/Ankka/Window.h"
	"Window.cpp" "Logger.cpp")



target_include_directories(Ankka
	PRIVATE 
		${CMAKE_SOURCE_DIR}/includes)

find_package(Vulkan REQUIRED)

target_link_libraries(Ankka 
	PRIVATE 
	glfw
	glad
	Vulkan::Vulkan)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Ankka PROPERTY CXX_STANDARD 20)
endif()


