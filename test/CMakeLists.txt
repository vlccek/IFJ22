project(IFJ-tests LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.14)

# Lexer test
set(This stackTest)
set(Sources stackTest.cpp)
add_executable(${This} ${Sources})
target_link_libraries(${This} gtest_main stack)
add_test(stackTest stackTest)