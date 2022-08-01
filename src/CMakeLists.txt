project(procyon)

add_library(procyon
	procyon.c
)

target_link_libraries(${PROJECT_NAME}
	glad
	glfw
)

target_include_directories(procyon
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

