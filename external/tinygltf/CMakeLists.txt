add_library(tinygltf STATIC tiny_gltf.cc)

target_include_directories(tinygltf 
					PUBLIC
						${CMAKE_CURRENT_SOURCE_DIR}
						${PROJECT_SOURCE_DIR})


