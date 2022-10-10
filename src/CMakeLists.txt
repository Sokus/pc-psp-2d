add_library(procyon
    procyon.c
    procyon_gfx.c
    procyon_texture.c
    procyon_math.c
)

if(WIN32)
    target_link_libraries(procyon PRIVATE glad glfw)
endif()

if(PSP)
    target_link_libraries(procyon PUBLIC pspdebug pspdisplay pspge pspgu pspgum pspctrl)
endif()

target_link_libraries(procyon PRIVATE stb::image
)

target_include_directories(procyon
    PUBLIC
	    ${CMAKE_CURRENT_SOURCE_DIR}
)

# if (MSVC)
#     target_compile_options(procyon PUBLIC /W4 -D _CRT_SECURE_NO_WARNINGS)
# else()
#     target_compile_options(procyon PUBLIC -Wall -Wextra -pedantic)
# endif()