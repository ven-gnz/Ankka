# GLFW Externally compiled lib, cascaded down from external folder

add_library(glfw SHARED IMPORTED GLOBAL)

set_target_properties(glfw PROPERTIES
	IMPORTED_LOCATION
		${CMAKE_CURRENT_LIST_DIR}/lib/glfw3.dll

	IMPORTED_IMPLIB
		${CMAKE_CURRENT_LIST_DIR}/lib/glfw3.lib

	INTERFACE_INCLUDE_DIRECTORIES
		${CMAKE_CURRENT_LIST_DIR}/include
)
