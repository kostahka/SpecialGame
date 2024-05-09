cmake_minimum_required(VERSION 3.5)

set(SPECIAL_GAME_SOURCES
    # cmake-format: sortable
    components/astronaut-component.cxx
    components/player-controller.cxx
    game.cxx
    resources/resources.cxx
    system/astronaut-system.cxx
    system/landscape-system.cxx
    system/planet-gameplay-system.cxx
    system/player-system.cxx
)

set(SPECIAL_GAME_HEADERS
    # cmake-format: sortable
    ../include/astronaut/gun.hxx
    ../include/components/astronaut-component.hxx
    ../include/components/player-controller.hxx
    ../include/game.hxx
    ../include/resources/resources.hxx
    ../include/system/astronaut-system.hxx
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
