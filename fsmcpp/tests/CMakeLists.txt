
include_directories(
    ../../thirdparty
    ../../thirdparty/googletest-1.10.0/include)

link_directories(
    ../../thirdparty/googletest-1.10.0/lib64
)

add_executable(fsm_unittest fsm_unittest.cpp)
target_link_libraries(fsm_unittest gtest_main gtest pthread)
