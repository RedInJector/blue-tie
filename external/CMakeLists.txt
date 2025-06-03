cmake_minimum_required(VERSION 3.29)

set(DISCORD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/discord_game_sdk")
set(DISCORD_LIB "${DISCORD_PATH}/lib/x86_64/discord_game_sdk.dll.lib" )
set(DISCORD_DLL "${DISCORD_PATH}/lib/x86_64/discord_game_sdk.dll" )
if(NOT EXISTS "${DISCORD_LIB}")
    message(FATAL_ERROR "Discord SDK library not found at: ${DISCORD_LIB}")
endif()
if(NOT EXISTS "${DISCORD_DLL}")
    message(FATAL_ERROR "Discord SDK library not found at: ${DISCORD_DLL}")
endif()

set(DISCORD_LIB "${DISCORD_PATH}/lib/x86_64/discord_game_sdk.dll.lib" PARENT_SCOPE)
set(DISCORD_DLL "${DISCORD_PATH}/lib/x86_64/discord_game_sdk.dll" PARENT_SCOPE)

file(GLOB DISCORD_SOURCES ${DISCORD_PATH}/cpp/*.cpp)
add_library(Discord STATIC ${DISCORD_SOURCES})
target_include_directories(Discord PUBLIC ${DISCORD_PATH}/cpp)
target_link_libraries(Discord ${DISCORD_LIB})
