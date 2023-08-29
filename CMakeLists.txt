cmake_minimum_required(VERSION 3.26)
project(TinyTim C)

set(CMAKE_C_STANDARD 11)

find_library(libconfig libconfig)
add_compile_options(-lconfig)

file(MAKE_DIRECTORY /etc/TinyTim)
file(MAKE_DIRECTORY /usr/share/TinyTim/www)

add_executable(
        TinyTim
        src/main.c
        src/main.h
)

target_link_libraries(TinyTim config)

install(
        TARGETS TinyTim
        DESTINATION bin
)

install (
        FILES ./etc/TinyTim/tinytim.conf
        DESTINATION /etc/TinyTim/
)


