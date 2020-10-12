enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(test_scheduler test_main.cpp test_validation.cpp "test_solving.cpp" "test_main.h")
target_link_libraries(test_scheduler GTest::gtest GTest::gtest_main scheduler_lib)

target_compile_definitions(test_scheduler PUBLIC TEST_RESOURCE_DIR="${CMAKE_CURRENT_LIST_DIR}/res/")

add_test(NAME AllTests COMMAND test_scheduler )