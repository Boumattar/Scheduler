enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(test_scheduler test_main.cpp test_validation.cpp)
target_link_libraries(test_scheduler GTest::gtest GTest::gtest_main scheduler_lib)

add_test(AllTests test_scheduler)