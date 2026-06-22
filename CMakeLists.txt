cmake_minimum_required(VERSION 3.22)
project(CubicBattle)

set(CMAKE_CXX_STANDARD 17)

add_library(cubicbattle SHARED
    main.cpp
)

target_link_libraries(cubicbattle
    log
    android
    EGL
    GLESv2
)
