cmake_minimum_required (VERSION 3.5)
project(procyon)

add_subdirectory(external)

add_subdirectory(src)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(PROCYON_STANDALONE TRUE)
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PROCYON_BUILD_EXAMPLES "Build the PROCYON example programs" ${PROCYON_STANDALONE})

if(${PROCYON_STANDALONE})
    add_subdirectory(examples)
endif()