cmake_minimum_required (VERSION 3.5)
project(procyon VERSION 0.1.0)

add_subdirectory(lib)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_library(procyon src/procyon.c)

if(WIN32)
    target_link_libraries(procyon
        glad
        glfw
    )
endif()

target_link_libraries(procyon
    cglm
)

target_include_directories(procyon
    PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/procyon
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}
)

add_executable(test src/test.c)
target_link_libraries(test PRIVATE procyon)

# if (MSVC)
#     target_compile_options(procyon PUBLIC /W4 -D _CRT_SECURE_NO_WARNINGS)
# else()
#     target_compile_options(procyon PUBLIC -Wall -Wextra -pedantic)
# endif()
