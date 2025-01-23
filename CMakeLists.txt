cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

project(pmc C)

add_executable(pmc
    "source/main.c"
    "source/parse.c"
    "source/print.c"
)
include_directories(
    "source"
)

target_compile_options(pmc PUBLIC -g)