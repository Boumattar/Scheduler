#include "test_main.h"
#include "gtest/gtest.h"

std::string input_test_json_file_name;
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}
