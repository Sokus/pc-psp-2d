project(examples)

add_executable(hello_procyon hello_procyon.c)
target_link_libraries(hello_procyon procyon)