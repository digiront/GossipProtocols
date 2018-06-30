cmake_minimum_required(VERSION 3.0)

file(GLOB SOURCES *.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TestSystem ${SOURCES})
target_link_libraries(TestSystem
        PUBLIC
        libgtest
        libgmock
        libRumorSpreading)
add_test(NAME   TestSystem
        COMMAND TestSystem)
