cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(special-game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ENGINE_DEV ON CACHE BOOL "engine load game dll with hot reload")
set(ENGINE_GAME_LIB_NAME "special-game" CACHE STRING "game lib name to load")

set(GAME_SRC "src/game.cxx")

add_library(special-game SHARED ${GAME_SRC})

add_subdirectory(engine)

target_link_libraries(special-game PRIVATE engine)
