set(CMAKE_C_STANDARD 11)

add_executable(procyon_app main.c)
target_link_libraries(procyon_app
    PRIVATE
        procyon
)

if(PSP)
    create_pbp_file(
        TARGET procyon_app
        ICON_PATH NULL
        PREVIEW_PATH NULL
        TITLE "Procyon App"
        BUILD_PRX
    )
endif()
