cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(TestProtocol TestProtocol.cpp)
target_link_libraries(TestProtocol
        PUBLIC
        libgtest
        libgmock
        libRumorSpreading)
add_test(NAME TestProtocol
        COMMAND TestProtocol)
