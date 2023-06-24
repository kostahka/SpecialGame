cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(engine)

option(BUILD_SHARED_LIBS OFF)
if(SDL3_SRC_DIR AND ANDROID)
    add_subdirectory(
        # Specifies the directory of the CMakeLists.txt file.
        ${SDL3_SRC_DIR}
        # Specifies the directory for the build outputs.
        ${CMAKE_CURRENT_BINARY_DIR}/SDL_${ANDROID_ABI})
else()
    find_package(SDL3 REQUIRED)
endif()

option(BOX2D_BUILD_UNIT_TESTS OFF)
option(BOX2D_BUILD_TESTBED OFF)
option(BOX2D_BUILD_DOCS ON)
option(BOX2D_USER_SETTINGS OFF)

add_subdirectory(efsw)
add_subdirectory(glm)
add_subdirectory(box2d)
set_property(TARGET box2d PROPERTY POSITION_INDEPENDENT_CODE ON)

file(GLOB IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.cpp)
file(GLOB IMGUI_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.h)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(ENGINE_DEV OFF CACHE BOOL "engine load game dll with hot reload")
set(ENGINE_GAME_LIB_NAME "" CACHE STRING "game lib name to load")

set(GLAD_SRC "glad/src/glad.c")

set(ENGINE_SRC
    src/engine.cxx
    src/render/shader-program.cxx
    src/render/vertex-array-object.cxx
    src/file-last-modify-listener.cxx
    src/event/user-events.cxx
    src/event/handle-user-event.hxx
    src/handle-file-modify.hxx
    src/render/picopng.hxx
    src/render/texture.cxx
    src/render/sprite.cxx
    include/Kengine/render/sprite.hxx
    include/Kengine/render/texture.hxx
    include/Kengine/event/user-events.hxx
    include/Kengine/file-last-modify-listener.hxx
    include/Kengine/engine.hxx
    include/Kengine/render/shader-program.hxx
    include/Kengine/render/vertex-array-object.hxx
    include/Kengine/render/color.hxx
    include/Kengine/transform3d.hxx
    include/Kengine/render/vertex.hxx
    ${GLAD_SRC}
    src/render/opengl-error.hxx
    src/render/opengl-error.cxx
    ${IMGUI_SRC}
    ${IMGUI_INCLUDE}
    include/Kengine/render/b2GLDraw.hxx
    src/render/b2GLDraw.cxx
    include/Kengine/render/draw-primitives.hxx
    src/render/draw-primitives.cxx
    include/Kengine/render/engine-resources.hxx
    src/render/engine-resources.cxx
    include/Kengine/io/input.hxx
    src/io/input.cxx
    include/Kengine/event/event.hxx
    src/event/event.cxx
    src/event/event-engine.hxx
    include/Kengine/render/animation.hxx
    src/render/animation.cxx
    src/transform3d.cxx
    src/audio/audio.hxx
    src/audio/audio.cxx
    include/Kengine/audio/audio.hxx
    include/Kengine/io/file-manager.hxx
    src/io/file-manager.cxx)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/glad/include)

if(ENGINE_DEV)
    if(NOT ANDROID)
        add_executable(engine ${ENGINE_SRC})
    else()
        add_library(engine SHARED ${ENGINE_SRC})
    endif()
    target_link_libraries(engine PRIVATE SDL3::SDL3-static)
    target_compile_definitions(engine PUBLIC ENGINE_DEV)
    target_compile_definitions(
        engine PUBLIC ENGINE_GAME_LIB_NAME="${ENGINE_GAME_LIB_NAME}")

    target_compile_definitions(engine PUBLIC IMGUI_IMPL_OPENGL_ES3)

    if(WIN32)
        target_compile_definitions(engine
                                   PRIVATE "-DE_DECLSPEC=__declspec(dllexport)")
    else()
        target_compile_definitions(engine PRIVATE "-DE_DECLSPEC=")
    endif()

    target_link_libraries(engine PRIVATE efsw)
    target_link_libraries(engine PUBLIC glm::glm)
    target_link_libraries(engine PUBLIC box2d)

    target_include_directories(engine
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
    target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

    set_target_properties(engine PROPERTIES ENABLE_EXPORTS ON)

    if(ANDROID)
        target_link_libraries(
            engine
            PUBLIC android
                   log
                   EGL
                   GLESv3)
    endif()
else()

    # Shared engine lib
    add_library(game-engine-shared SHARED ${ENGINE_SRC})
    target_link_libraries(game-engine-shared PRIVATE SDL3::SDL3-static)

    target_link_libraries(game-engine-shared PRIVATE efsw)
    target_link_libraries(game-engine-shared PUBLIC glm::glm)
    target_link_libraries(game-engine-shared PUBLIC box2d)

    target_compile_definitions(game-engine-shared PUBLIC IMGUI_IMPL_OPENGL_ES3)

    if(WIN32)
        target_compile_definitions(game-engine-shared
                                   PRIVATE "-DE_DECLSPEC=__declspec(dllexport)")
    else()
        target_compile_definitions(game-engine-shared PRIVATE "-DE_DECLSPEC=")
    endif()

    target_include_directories(game-engine-shared
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
    target_include_directories(game-engine-shared
                               PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

    set_property(TARGET game-engine-shared PROPERTY POSITION_INDEPENDENT_CODE
                                                    ON)

    if(ANDROID)
        target_link_libraries(
            game-engine-shared
            PUBLIC android
                   log
                   EGL
                   GLESv3)
    endif()
    # Static engine lib
    add_library(game-engine-static STATIC ${ENGINE_SRC})
    target_link_libraries(game-engine-static PRIVATE SDL3::SDL3-static)

    target_link_libraries(game-engine-static PRIVATE efsw)
    target_link_libraries(game-engine-static PUBLIC glm::glm)
    target_link_libraries(game-engine-static PUBLIC box2d)

    target_compile_definitions(game-engine-static PUBLIC IMGUI_IMPL_OPENGL_ES3)

    if(WIN32)
        target_compile_definitions(game-engine-static
                                   PRIVATE "-DE_DECLSPEC=__declspec(dllexport)")
    else()
        target_compile_definitions(game-engine-static PRIVATE "-DE_DECLSPEC=")
    endif()

    target_include_directories(game-engine-static
                               PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src)
    target_include_directories(game-engine-static
                               PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

    set_property(TARGET game-engine-static PROPERTY POSITION_INDEPENDENT_CODE
                                                    ON)

    if(ANDROID)
        target_link_libraries(
            game-engine-static
            PUBLIC android
                   log
                   EGL
                   GLESv3)
    endif()
endif()
