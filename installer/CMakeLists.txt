set(INSTALLER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BLUE_TIE_BINARY "$<TARGET_FILE:blue_tie>")
set(DEST_BINARY "${INSTALLER_DIR}/$<TARGET_FILE_NAME:blue_tie>")

message(STATUS "${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(prepare_installer ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Copying blue_tie to installer folder..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLUE_TIE_BINARY} ${DEST_BINARY}
            COMMAND ${CMAKE_COMMAND} -E echo "Copying discord_game_sdk.dll to installer folder..."
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DISCORD_DLL} ${INSTALLER_DIR}
            COMMAND ${CMAKE_COMMAND} -E echo "Running dotnet build in installer folder..."
            COMMAND dotnet build "${INSTALLER_DIR}"
            WORKING_DIRECTORY "${INSTALLER_DIR}"
            DEPENDS blue_tie
            COMMENT "Preparing installer: copying binary and building WIX project"
    )
else ()
    add_custom_target(prepare_installer ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Target should only be run in Release build"
    )
endif ()
set_target_properties(prepare_installer PROPERTIES EXCLUDE_FROM_ALL TRUE)