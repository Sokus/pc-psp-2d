cmake_minimum_required (VERSION 3.5)
project(procyon)

add_subdirectory(deps)

file(GLOB PROCYON_SRC src/*.c)
add_library(${PROJECT_NAME}
    ${PROCYON_SRC}
)

target_link_libraries(${PROJECT_NAME}
    glad
    glfw
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(PROCYON_STANDALONE TRUE)
endif()
option(PROCYON_BUILD_EXAMPLES "Build Procyon usage examples" ${PROCYON_STANDALONE})

if(PROCYON_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()