cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(special-game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(ENGINE_DEV OFF CACHE BOOL "engine load game dll with hot reload")
set(ENGINE_GAME_LIB_NAME "special-game" CACHE STRING "game lib name to load")

set(GAME_SRC
    src/game.cxx
    include/game.hxx
    src/landscape.cxx
    include/landscape.hxx
    include/resources.hxx
    src/resources.cxx
    src/render/camera.cxx
    include/render/camera.hxx
    include/render/cursor.hxx
    src/render/cursor.cxx
    include/scene/scene.hxx
    include/scene/planet-scene.hxx
    src/scene/planet-scene.cxx
    src/scene/scene.cxx
    include/physics/physics.hxx
    src/physics/physics.cxx
    include/physics/physic-object.hxx
    src/physics/physics-object.cxx
    include/astronaut/astronaut.hxx
    src/astronaut/astronaut.cxx
    include/game-object/game-object.hxx
    include/astronaut/player.hxx
    src/astronaut/player.cxx
    include/astronaut/bullet.hxx
    src/astronaut/bullet.cxx
    src/game-object/game-object.cxx
    include/physics/collision_interface.hxx
    include/astronaut/enemy.hxx
    include/game-object/controller.hxx
    src/game-object/controller.cxx
    include/game-object/destroy_listener.hxx
    src/astronaut/enemy.cxx
    src/astronaut/enemy_spawner.cxx
    include/astronaut/enemy_spawner.hxx
    src/game-object/destroy_listener.cxx
    include/render/game-gui.hxx
    src/render/background.cxx
    include/render/background.hxx
    src/render/game-gui.cxx
    src/scene/main-menu.cxx
    include/scene/main-menu.hxx
    include/joystick.hxx
    src/joystick.cxx
    include/astronaut/gun.hxx)

if(ENGINE_DEV)
    add_library(special-game SHARED ${GAME_SRC})
else()
    if(ANDROID)
        add_library(special-game SHARED ${GAME_SRC})
    else()
        add_executable(special-game ${GAME_SRC})
    endif()
endif()

if(WIN32)
    target_compile_definitions(special-game
                               PRIVATE "-DE_DECLSPEC=__declspec(dllimport)")
else()
    target_compile_definitions(special-game PRIVATE "-DE_DECLSPEC=")
endif()

add_subdirectory(engine)

if(ANDROID)
    add_custom_target(
        copy_assets_shaders ALL
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/shaders/
            ${CMAKE_CURRENT_LIST_DIR}/android-project/app/src/main/assets/shaders
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/
            ${CMAKE_CURRENT_LIST_DIR}/android-project/app/src/main/assets/assets
        COMMENT "Copying shaders and assets into binary directory")
else()
    add_custom_target(
        copy_assets_shaders ALL
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_LIST_DIR}/shaders/
            ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMAND
            ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets/
            ${CMAKE_CURRENT_BINARY_DIR}/assets
        COMMENT "Copying shaders and assets into binary directory")
endif()
add_dependencies(special-game copy_assets_shaders)

if(ENGINE_DEV)
    target_link_libraries(special-game PRIVATE engine)
else()
    target_link_libraries(special-game PRIVATE game-engine-static)
endif()
target_include_directories(special-game
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
