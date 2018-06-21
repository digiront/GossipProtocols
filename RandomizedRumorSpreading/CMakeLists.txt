project(RandomizedRumorSpreading)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 14)

include_directories("${PROJECT_SOURCE_DIR}/libRumorSpreading")
add_subdirectory(libRumorSpreading)

add_executable(RandomizedRumorSpreading main.cpp)
target_link_libraries(RandomizedRumorSpreading libRumorSpreading)