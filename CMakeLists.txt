cmake_minimum_required (VERSION 3.5)
project(procyon VERSION 0.1.0)

add_subdirectory(deps)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(PROCYON_STANDALONE TRUE)
endif()

option(PLATFORM_DESKTOP "Platform Desktop" ON)
option(ENABLE_LOG "Enable Logs" ON)
option(ENABLE_DEBUG "Enable Debug" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PROCYON_BUILD_EXAMPLES "Build examples" ${PROCYON_STANDALONE})

message(STATUS "[procyon] Platform Desktop: ${PLATFORM_DESKTOP}")
message(STATUS "[procyon] Enable Logs: ${ENABLE_LOG}")
message(STATUS "[procyon] Enable Debug: ${ENABLE_DEBUG}")
message(STATUS "[procyon] Build shared libraries: ${BUILD_SHARED_LIBS}")
message(STATUS "[procyon] Build examples: ${PROCYON_BUILD_EXAMPLES}")

configure_file(cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)

file(GLOB PROCYON_SRC src/*.c)
add_library(${PROJECT_NAME}
    ${PROCYON_SRC}
)

if(PLATFORM_DESKTOP)
    target_link_libraries(${PROJECT_NAME}
        glad
        glfw
    )
endif()

target_link_libraries(${PROJECT_NAME}
    cglm
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/procyon
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /W4 -D _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -pedantic)
endif()

if(PROCYON_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
