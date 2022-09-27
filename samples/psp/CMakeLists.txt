add_executable(psp_app main.c)
target_link_libraries(psp_app
    PRIVATE
        procyon
        pspdebug pspdisplay pspge pspgu pspgum pspctrl
        stb::image
)

create_pbp_file(
    TARGET psp_app
    ICON_PATH NULL
    PREVIEW_PATH NULL
    TITLE "Procyon App"
    BUILD_PRX
)
