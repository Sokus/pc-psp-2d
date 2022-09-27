add_executable(app_desktop main.c)
target_link_libraries(app_desktop
    PRIVATE
        procyon
)