project(IFJ-tests LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)

# stack Tests
set(This stackTest)
set(Sources stackTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main stack)
add_test(stackTest stackTest)
# symtable Tests
set(This symtableTest)
set(Sources symtableTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main symtable)
add_test(symtableTest symtableTest)

# dynstring test
set(This dynStrTest)
set(Sources dynStrTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main dstring)
add_test(dynStrTest dynStrTest)

# Parser test
set(This parserTest)
set(Sources parserTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main parser lex common stack LLtable dstring semanticActions expParse)
target_compile_definitions(${This} PUBLIC TESTING=1)
add_test(parserTest parserTest)

#expparser
set(This expparserTest)
set(Sources expParser.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main expParseTests lex)
add_test(expparserTest expparserTest)