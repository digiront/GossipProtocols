file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(unit_tests ${SRCS})

TARGET_LINK_LIBRARIES(
    unit_tests
    libgtest
    libRumorSpreading)

add_test(NAME unit_tests
         COMMAND unit_tests)