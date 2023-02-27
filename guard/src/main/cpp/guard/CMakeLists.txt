add_library(
        guard
        SHARED
        so_protector.cpp
        guard.cpp
        event.cpp
        mmap_ext.cpp
)
find_library(
        log-lib
        log )
find_package(common REQUIRED CONFIG)
target_link_libraries(
        guard
        common::util
        ${log-lib} )