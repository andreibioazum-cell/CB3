cmake_minimum_required(VERSION 3.22)
project(CubicBattle)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

add_library(cubicbattle SHARED
    main.cpp
    lobby.cpp
    game.cpp
    enemy.cpp
)

target_include_directories(cubicbattle PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(cubicbattle PRIVATE _GNU_SOURCE)

target_link_libraries(cubicbattle
    log
    android
    EGL
    GLESv3  # <-- GLESv3 вместо GLESv2!
)
