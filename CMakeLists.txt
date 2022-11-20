cmake_minimum_required(VERSION 3.16)
project(stack C)

cmake_policy(SET CMP0135 NEW)


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
        dynstring.h)


add_library(parser
        parser.c
        parser.h
        lex.h
        lex.c)

add_library(semanticActions
        semanticActions.c
        semanticActions.h)

add_library(expParse
        expParse.c
        expParse.h)

add_library(common
        common.c
        common.h)

add_library(lex
        lex.c
        lex.h
        dynstring)

add_library(lextest
        lex.c
        lex.h)

add_library(LLtable
        LLtable.c
        LLtable.h)

add_library(symtable
        symtable.c
        symtable.h)

add_library(expParseTests
        expParse.c
        expParse.h
        dynstring.c
        dynstring.h
        LLtable
        parser
        common
        stack
        lex)

add_library(generator
        generator/generator.c
        generator/generator.h
        generator/igen.c
        generator/igen.h
        generator/3adres.c
        generator/3adres.h
)


target_compile_definitions(expParseTests PUBLIC)

add_subdirectory(test)

target_link_libraries(main
        stack
        queue
        dstring
        LLtable
        lex
        parser
        semanticActions
        expParse
        common
        symtable
        generator
        )


