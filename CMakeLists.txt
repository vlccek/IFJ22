cmake_minimum_required(VERSION 3.22)
project(stack C)


set(CMAKE_C_STANDARD 11)
add_executable(main
        main.c
        )

add_library(stack
        stack.c)


target_link_libraries(main
        stack)


