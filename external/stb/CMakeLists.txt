project(stb)

add_library(stb_image stb_image.c)
add_library(stb::image ALIAS stb_image)
target_include_directories(stb_image PUBLIC ${PROJECT_SOURCE_DIR})