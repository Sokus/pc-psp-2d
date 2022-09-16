project(stb)

add_library(stb_image INTERFACE)
add_library(stb::image ALIAS stb_image)

target_include_directories(stb_image INTERFACE ${PROJECT_SOURCE_DIR})