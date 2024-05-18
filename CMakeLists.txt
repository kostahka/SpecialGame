cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(special-game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/Kengine)

add_subdirectory(src)

if(ANDROID)
    add_custom_target(
        copy_special_game_assets ALL
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/
            ${CMAKE_CURRENT_LIST_DIR}/android-project/app/src/main/assets/assets
        COMMENT "Copying assets into binary directory"
    )
else()
    add_custom_target(
        copy_special_game_assets ALL
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/
            $<TARGET_FILE_DIR:special-game>/assets
        COMMENT
            "Copying assets into game directory: [$<TARGET_FILE_DIR:special-game>]"
    )
endif()
