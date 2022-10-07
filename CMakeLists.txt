cmake_minimum_required(VERSION 3.20)
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

add_compile_definitions("DEBUG=$<CONFIG:Debug>")


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

add_library(dstring
        dynstring.c
        dynstring.c)

add_library(parser
        parser.c
        parser.h)


add_library(common
        common.c
        common.h)

add_library(lex
        lex.c
        lex.h)

add_library(LLtable
        LLtable.c
        LLtable.h)

add_library(expParseTests
        expParse.c
        expParse.h
        LLtable
        parser
        common
        stack
        lex)


target_compile_definitions(expParseTests PUBLIC TESTING=1)


add_subdirectory(test)

target_link_libraries(main
        stack
        queue
        dstring
        LLtable)


