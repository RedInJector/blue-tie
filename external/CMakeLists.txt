cmake_minimum_required(VERSION 3.30)

set(DISCORD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/discord")
file(GLOB DISCORD_SOURCES ${DISCORD_PATH}/src/*.cpp)
add_library(Discord STATIC ${DISCORD_SOURCES})
target_include_directories(Discord PUBLIC ${DISCORD_PATH}/src)
target_link_libraries(Discord ${DISCORD_PATH}/lib/discord_game_sdk.dll.lib)
