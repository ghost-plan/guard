cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

find_library(
        log-lib
        log )
set(mathc_src ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/NativeMath.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/Initializer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/LibraryDecryptor.cpp
        )
add_library(mathc SHARED ${mathc_src})
target_link_libraries(
        mathc
        ${log-lib}
#        cutils
)

set(unload_src ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/UnloadTest.cpp)
add_library(unload SHARED ${unload_src})
target_link_libraries(
        unload
        ${log-lib}
#        cutils
)