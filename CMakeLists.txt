cmake_minimum_required(VERSION 3.16)
project(IFJ22 C)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
cmake_policy(SET CMP0135 NEW)
endif()


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
        LLtable.c
        common.c
        dynstring.c
        expParse.c
        gen_3adres.c
        gen_gen.c
        gen_igen.c
        lex.c
        main.c
        parser.c
        queue.c
        semanticActionInfo.c
        semanticActions.c
        stack.c
        symtable.c
        LLtable.h
        common.h
        dynstring.h
        expParse.h
        gen_3adres.h
        gen_gen.h
        gen_igen.h
        lex.h
        parser.h
        queue.h
        semanticActionInfo.h
        semanticActions.h
        stack.h
        symtable.h
        stackIf.c
        stackIf.h
        gen_postproces.c
        gen_postproces.h
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
        parser.h)

add_library(generator
        gen_3adres.c
        gen_3adres.h
        gen_gen.c
        gen_gen.h
        gen_igen.c
        gen_igen.h)

add_library(semanticActions
        gen_3adres.c
        gen_3adres.h
        gen_gen.c
        gen_gen.h
        gen_igen.c
        gen_igen.h
        semanticActions.c
        semanticActions.h semanticActionInfo.c semanticActionInfo.h)

add_library(expParse
        expParse.c
        expParse.h)

add_library(common
        common.c
        common.h)

add_library(lex
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


target_compile_definitions(expParseTests PUBLIC)

add_subdirectory(test)

target_link_libraries(parser
        lex
        semanticActions
        common
        expParse
        stack
        symtable)

target_link_libraries(semanticActions
        LLtable)

target_link_libraries(expParse
        LLtable
        stack
        common
        stack)

target_link_libraries(dstring
        common)

target_link_libraries(lex
        common
        dstring)

target_link_libraries(LLtable
        common
        lex)

target_link_libraries(queue
        stack)

target_link_libraries(stack
        common)

target_link_libraries(symtable
        common
        lex)
