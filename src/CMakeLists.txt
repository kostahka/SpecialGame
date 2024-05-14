cmake_minimum_required(VERSION 3.5)

set(CMAKE_UNITY_BUILD ON)

set(SPECIAL_GAME_SOURCES
    # cmake-format: sortable
    components/astronaut-component.cxx
    components/bullet-component.cxx
    components/enemies-progress-bar-component.cxx
    components/enemy-component.cxx
    components/enemy-wave-text-component.cxx
    components/game-over-menu-component.cxx
    components/game-tutorial-menu-component.cxx
    components/gui-button-component.cxx
    components/gui-selectable-component.cxx
    components/pause-menu-component.cxx
    components/player-controller.cxx
    components/player-gun-selectable-component.cxx
    components/player-health-bar-component.cxx
    components/progress-bar-component.cxx
    game.cxx
    resources/resources.cxx
    system/astronaut-system.cxx
    system/background-system.cxx
    system/bullet-system.cxx
    system/enemy-spawner-system.cxx
    system/enemy-system.cxx
    system/game-gui-system.cxx
    system/landscape-system.cxx
    system/planet-gameplay-system.cxx
    system/player-system.cxx
)

set(SPECIAL_GAME_HEADERS
    # cmake-format: sortable
    ../include/astronaut/gun.hxx
    ../include/components/astronaut-component.hxx
    ../include/components/bullet-component.hxx
    ../include/components/damage-interface.hxx
    ../include/components/enemies-progress-bar-component.hxx
    ../include/components/enemy-component.hxx
    ../include/components/enemy-wave-text-component.hxx
    ../include/components/game-over-menu-component.hxx
    ../include/components/game-tutorial-menu-component.hxx
    ../include/components/gui-button-component.hxx
    ../include/components/gui-selectable-component.hxx
    ../include/components/pause-menu-component.hxx
    ../include/components/player-controller.hxx
    ../include/components/player-gun-selectable-component.hxx
    ../include/components/player-health-bar-component.hxx
    ../include/components/progress-bar-component.hxx
    ../include/game.hxx
    ../include/resources/resources.hxx
    ../include/system/astronaut-system.hxx
    ../include/system/background-system.hxx
    ../include/system/bullet-system.hxx
    ../include/system/enemy-spawner-system.hxx
    ../include/system/enemy-system.hxx
    ../include/system/game-gui-system.hxx
    ../include/system/landscape-system.hxx
    ../include/system/planet-gameplay-system.hxx
    ../include/system/player-system.hxx
)

if(ANDROID)
    add_library(
        special-game SHARED ${SPECIAL_GAME_SOURCES} ${SPECIAL_GAME_HEADERS}
    )
else()
    add_executable(special-game ${SPECIAL_GAME_SOURCES} ${SPECIAL_GAME_HEADERS})
endif()

developgame(
    special-game-dev "${SPECIAL_GAME_SOURCES}" "${SPECIAL_GAME_HEADERS}"
)

target_include_directories(
    special-game-dev PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_include_directories(
    special-game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(special-game PRIVATE Kengine::kengine-static)
