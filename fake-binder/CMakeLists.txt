cmake_minimum_required(VERSION 3.6)
set(CMAKE_VERBOSE_MAKEFILE on)
project(binder VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -Wno-format -Wno-sign-compare -Wno-unused-parameter -DHAVE_CONFIG_H")
add_definitions(-DENABLE_ANDROID_LOG)

file(GLOB_RECURSE FAKE_BINDER_SRC "src/main/cpp/*.cpp")
add_library(
        fakebinder
        SHARED
        ${FAKE_BINDER_SRC}
)
find_package(common REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)
target_link_libraries(
        fakebinder
        common::util
        fbjni::fbjni
        log android
)