cmake_minimum_required (VERSION 3.5)
project(procyon)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_SAMPLES "Build samples" ON)

add_subdirectory(external)
add_subdirectory(src)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
