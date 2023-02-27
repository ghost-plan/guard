add_library( # Sets the name of the library.
        so_shell
        SHARED
        gdlfcn.cpp
        linker.cpp
        linker_allocator.cpp
        linker_phdr.cpp
        Shell.cpp
        Log.cpp
        Utils.cpp)
find_library( # Sets the name of the path variable.
        log-lib

        log)
target_link_libraries( # Specifies the target library.
        so_shell

        android z EGL jnigraphics mediandk
        linker
        ${log-lib})