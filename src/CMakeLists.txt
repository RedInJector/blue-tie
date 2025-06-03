

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
        set(POS WIN32)
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
    endif()
endif()

add_executable(blue_tie ${POS} main.cpp single_instance_guard.cpp tray_icon.cpp)
target_link_libraries(blue_tie Discord user32.lib shell32.lib)
