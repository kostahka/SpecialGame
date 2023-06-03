cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(special-game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(ENGINE_DEV ON CACHE BOOL "engine load game dll with hot reload")
set(ENGINE_GAME_LIB_NAME "special-game" CACHE STRING "game lib name to load")

set(GAME_SRC "src/game.cxx" "src/landscape.cxx")

add_library(special-game SHARED ${GAME_SRC})

add_subdirectory(engine)

add_custom_target(
    copy_assets_shaders ALL
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/shaders/
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/
        ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying shaders and assets into binary directory")

add_dependencies(special-game copy_assets_shaders)

target_link_libraries(special-game PRIVATE engine)
target_include_directories(special-game
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
