cmake_minimum_required(VERSION 3.30)
project(blue_tie)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(external)

add_executable(blue_tie main.cpp)
set_target_properties(blue_tie PROPERTIES
        WIN32_EXECUTABLE TRUE)
target_link_libraries(blue_tie Discord user32.lib shell32.lib)
