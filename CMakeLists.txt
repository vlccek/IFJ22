cmake_minimum_required(VERSION 3.22)
project(stack C)



include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
###################xx

enable_testing()




set(CMAKE_C_STANDARD 11)
add_executable(main
        main.c
        )

add_library(stack
        stack.c
        stack.h)

add_library(queue
        queue.c
        queue.h)

add_subdirectory(test)

target_link_libraries(main
        stack
        queue)


