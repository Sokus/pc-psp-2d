set(CMAKE_C_STANDARD 11)

if(WIN32)
    add_subdirectory(desktop)
elseif(PSP)
    add_subdirectory(psp)
endif()