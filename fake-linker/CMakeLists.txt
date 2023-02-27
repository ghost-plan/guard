cmake_minimum_required(VERSION 3.6)
set(CMAKE_VERBOSE_MAKEFILE on)
#project(linker VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -Wno-format -Wno-sign-compare -Wno-unused-parameter -DHAVE_CONFIG_H")
add_definitions(-DENABLE_ANDROID_LOG)

#==============================fake linker module======================================
#set(COMMON_PROJECT ${PROJECT_SOURCE_DIR}/../common/src/main/cpp)
#aux_source_directory(${COMMON_PROJECT}/fake_linker SOURCE_DIR_FAKE_LINKER)
#aux_source_directory(${COMMON_PROJECT}/fake_linker/build SOURCE_DIR_BUILD)
#aux_source_directory(${COMMON_PROJECT}/fake_linker/sig_safe_write SOURCE_DIR_SIG_SAFE_WRITE)
#aux_source_directory(${COMMON_PROJECT}/fake_linker/sigmux SOURCE_DIR_SIGMUX)
#aux_source_directory(${COMMON_PROJECT}/fake_linker/lss SOURCE_DIR_LSS)
#include_directories(${COMMON_PROJECT}/fake_linker/build)
#include_directories(${COMMON_PROJECT}/fake_linker/sigmux)
#include_directories(${COMMON_PROJECT}/fake_linker/sig_safe_write)
#include_directories(${COMMON_PROJECT}/fake_linker/lss)
add_library(
        fake_linker
        STATIC
        ${SOURCE_DIR_FAKE_LINKER} ${SOURCE_DIR_SIG_SAFE_WRITE} ${SOURCE_DIR_SIGMUX} ${SOURCE_DIR_BUILD} ${SOURCE_DIR_LSS})
target_link_libraries(fake_linker)


add_library(
        fake-linker
        SHARED
        src/main/cpp/OnLoad.cpp
)
find_package(common REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)
target_link_libraries(
        fake-linker
        common::util
        fbjni::fbjni
        log android
)