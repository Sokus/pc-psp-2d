cmake_minimum_required (VERSION 3.5)

add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

add_subdirectory(glfw)