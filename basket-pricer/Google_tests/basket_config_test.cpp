//
// Created by kca on 18/8/2019.
//

#include <iostream>
#include "gtest/gtest.h"
#include "basket_config.h"
#include "test_utils.h"

TEST(basket_config_test_suite, dummy_assert_true) {
    ASSERT_TRUE(true);
}

TEST(basket_config_test_suite, read_file_pass) {

    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/basket_defs.csv");

    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/basket_thresholds.csv");

    const std::shared_ptr<xyz::basket_config> basket_config =
        xyz::basket_config::read_files(basket_defs_file_path, basket_thresholds_file_path);

    ASSERT_EQ(basket_config->m_basket_name_to_entry_map.size(), 2);
    ASSERT_EQ(basket_config->m_ticker_set.size(), 4);
}

TEST(basket_config_test_suite, read_file_fail_file_not_found) {

    ASSERT_ANY_THROW(xyz::basket_config::read_files("/path/not/found", "/path/not/found2"));
}

TEST(basket_config_test_suite, read_file_fail_thresholds_missing_basket) {

    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/test/thresholds_missing_basket/basket_defs.csv");

    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/test/thresholds_missing_basket/basket_thresholds.csv");

    try {
        xyz::basket_config::read_files(basket_defs_file_path, basket_thresholds_file_path);
        ASSERT_TRUE(false) << "Unreachable code";
    }
    catch (std::invalid_argument& e) {
        std::cout << "std::invalid_argument: " << e.what() << std::endl;
        ASSERT_EQ(std::string(e.what()), xyz::basket_config::EXCEPTION_MESSAGE);
    }
}

TEST(basket_config_test_suite, read_file_fail_defs_missing_basket) {

    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/test/defs_missing_basket/basket_defs.csv");

    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/test/defs_missing_basket/basket_thresholds.csv");

    try {
        xyz::basket_config::read_files(basket_defs_file_path, basket_thresholds_file_path);
        ASSERT_TRUE(false) << "Unreachable code";
    }
    catch (std::invalid_argument& e) {
        std::cout << "std::invalid_argument: " << e.what() << std::endl;
        ASSERT_EQ(std::string(e.what()), xyz::basket_config::EXCEPTION_MESSAGE);
    }
}
