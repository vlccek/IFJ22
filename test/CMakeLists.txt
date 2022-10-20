project(IFJ-tests LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

# stack Tests
set(This stackTest)
set(Sources stackTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main stack)
add_test(stackTest stackTest)

#
set(This dynStrTest)
set(Sources dynStrTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main dstring)
add_test(dynStrTest dynStrTest)


set(This lexerTest)
set(Sources lexTests.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main lex)
add_test(lexerTest lexerTest)

# Parser test
set(This parserTest)
set(Sources parserTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main parser lex common stack LLtable dstring    )
target_compile_definitions(${This} PUBLIC TESTING=1)
add_test(parserTest parserTest)

#expparser
set(This expparserTest)
set(Sources expParser.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main expParseTests)
target_compile_definitions(${This} PUBLIC TESTING=1)
add_test(parserTest parserTest)