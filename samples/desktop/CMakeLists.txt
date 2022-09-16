add_executable(app_desktop main.c)
target_link_libraries(app_desktop
    PRIVATE
        procyon
)

set_property(TARGET app_desktop PROPERTY C_STANDARD 99)
set_property(TARGET app_desktop PROPERTY CXX_STANDARD 11)