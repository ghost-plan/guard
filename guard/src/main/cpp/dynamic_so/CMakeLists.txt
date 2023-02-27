add_library(
        dynamic_so
        SHARED
        main.cpp
)
find_library(
        log-lib
        log )

target_link_libraries(
        dynamic_so
        ${log-lib} )