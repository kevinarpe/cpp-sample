//
// Created by kca on 18/8/2019.
//

#include "gtest/gtest.h"
#include "test_utils.h"
#include "app_context.h"

TEST(system_test_suite, dummy_assert_true) {
    ASSERT_TRUE(true);
}

static void
pass_simple_single_basket_(std::shared_ptr<xyz::app_context> app_context) {

    const std::shared_ptr<xyz::basket_manager> basket_manager = app_context->get_basket_manager();
    const std::shared_ptr<xyz::basket> basket = basket_manager->get_basket("b1");
    const std::shared_ptr<xyz::basket_monitor> basket_monitor = app_context->get_basket_monitor();
    const std::shared_ptr<xyz::market_data_service> mds = app_context->get_market_data_service();

    /////////////////////
    // last price
    //
    ASSERT_EQ(0, basket->get_last_price_update_count());
    ASSERT_EQ(0, basket->get_last_price());
    ASSERT_EQ(0, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(0, basket_monitor->get_last_price("b1"));

    mds->update_last_price("c2", 55);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    // identical last price ignored
    mds->update_last_price("c2", 55);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    // zero price ignored
    mds->update_last_price("c2", 0);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    // negative price ignored
    mds->update_last_price("c2", -3.5);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    // unknown ticker ignored
    // filter @ market_data_service
    mds->update_last_price("c3", 75, xyz::validate_ticker::yes);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    // unknown ticker ignored
    // filter @ basket_manager
    mds->update_last_price("c3", 75, xyz::validate_ticker::no);

    ASSERT_EQ(1, basket->get_last_price_update_count());
    ASSERT_EQ(5500, basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    mds->update_last_price("c1", 12);

    ASSERT_EQ(2, basket->get_last_price_update_count());
    ASSERT_EQ(7900, basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(7900, basket_monitor->get_last_price("b1"));

    // tiny price change will not trigger monitor threshold
    mds->update_last_price("c1", 12.01);

    ASSERT_EQ(3, basket->get_last_price_update_count());
    ASSERT_EQ(7902, basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(7900, basket_monitor->get_last_price("b1"));

    /////////////////////
    // bid & ask prices
    //
    ASSERT_EQ(0, basket->get_bid_price_update_count());
    ASSERT_EQ(0, basket->get_bid_price());
    ASSERT_EQ(0, basket->get_ask_price_update_count());
    ASSERT_EQ(0, basket->get_ask_price());
    ASSERT_EQ(0, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(0, basket_monitor->get_mid_price("b1"));

    mds->update_bid_ask_prices("c1", 12, 13);

    ASSERT_EQ(1, basket->get_bid_price_update_count());
    ASSERT_EQ(2400, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    // identical bid & ask prices ignored
    mds->update_bid_ask_prices("c1", 12, 13);

    ASSERT_EQ(1, basket->get_bid_price_update_count());
    ASSERT_EQ(2400, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    // only change bid price; tiny bid price change will not trigger monitor threshold
    mds->update_bid_ask_prices("c1", 12.01, 13);

    ASSERT_EQ(2, basket->get_bid_price_update_count());
    ASSERT_EQ(2402, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    // only change bid price; bid price change will not trigger monitor threshold (99bps vs 100bps by config)
    mds->update_bid_ask_prices("c1", 12.2475, 13);

    ASSERT_EQ(3, basket->get_bid_price_update_count());
    ASSERT_EQ(2449.5, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    // only change bid price; bid price change will trigger monitor threshold
    mds->update_bid_ask_prices("c1", 12.5, 13);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // zero bid price ignored
    mds->update_bid_ask_prices("c1", 0, 13);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // zero ask price ignored
    mds->update_bid_ask_prices("c1", 12.5, 0);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // zero bid & ask prices ignored
    mds->update_bid_ask_prices("c1", 0, 0);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // negative bid price ignored
    mds->update_bid_ask_prices("c1", -7.5, 13);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // negative ask price ignored
    mds->update_bid_ask_prices("c1", 12.5, -3);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // negative bid & ask prices ignored
    mds->update_bid_ask_prices("c1", -7.5, -3);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // cross type #1: equal bid & ask prices ignored
    mds->update_bid_ask_prices("c1", 25, 25);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // cross type #2:  bid price greater than ask price ignored
    mds->update_bid_ask_prices("c1", 25.1, 25);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // unknown ticker ignored
    // filter @ market_data_service
    mds->update_bid_ask_prices("c3", 72, 73, xyz::validate_ticker::yes);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    // unknown ticker ignored
    // filter @ basket_manager
    mds->update_bid_ask_prices("c3", 72, 73, xyz::validate_ticker::no);

    ASSERT_EQ(4, basket->get_bid_price_update_count());
    ASSERT_EQ(2500, basket->get_bid_price());
    ASSERT_EQ(1, basket->get_ask_price_update_count());
    ASSERT_EQ(2600, basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2550, basket_monitor->get_mid_price("b1"));

    mds->update_bid_ask_prices("c2", 56, 57);

    ASSERT_EQ(5, basket->get_bid_price_update_count());
    ASSERT_EQ(8100, basket->get_bid_price());
    ASSERT_EQ(2, basket->get_ask_price_update_count());
    ASSERT_EQ(8300, basket->get_ask_price());
    ASSERT_EQ(3, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(8200, basket_monitor->get_mid_price("b1"));
}

TEST(system_test_suite, pass_simple_single_basket) {

    // b1,c2,100
    // b1,c1,200
    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/test/simple_single_basket/basket_defs.csv");

    // b1,3.0,5.0
    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/test/simple_single_basket/basket_thresholds.csv");

    std::shared_ptr<xyz::app_context> app_context =
        xyz::app_context::read_files(basket_defs_file_path, basket_thresholds_file_path);

    // Smoke test the config to make sure it is reasonable.
    ASSERT_EQ(app_context->get_basket_config()->m_basket_name_to_entry_map.size(), 1);
    ASSERT_EQ(app_context->get_basket_config()->m_ticker_set.size(), 2);

    pass_simple_single_basket_(app_context);
}

TEST(system_test_suite, pass_two_baskets_with_shared_ticker) {

    // b1,c2,100
    // b1,c1,200
    // b2,c1,50
    // b2,c3,25
    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/test/two_baskets_with_shared_ticker/basket_defs.csv");

    // b1,3.0,5.0
    // b2,1.0,2.0
    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/test/two_baskets_with_shared_ticker/basket_thresholds.csv");

    std::shared_ptr<xyz::app_context> app_context =
        xyz::app_context::read_files(basket_defs_file_path, basket_thresholds_file_path);

    // Smoke test the config to make sure it is reasonable.
    ASSERT_EQ(app_context->get_basket_config()->m_basket_name_to_entry_map.size(), 2);
    ASSERT_EQ(app_context->get_basket_config()->m_ticker_set.size(), 3);

    // demonstrate adding another basket with shared ticker does not interfere
    // all single basket tests continue to pass
    pass_simple_single_basket_(app_context);
}

TEST(system_test_suite, pass_two_baskets_with_shared_ticker2) {

    // b1,c2,100
    // b1,c1,200
    // b2,c1,50
    // b2,c3,25
    const std::string basket_defs_file_path =
        xyz::test_utils::resolve_path("data/test/two_baskets_with_shared_ticker/basket_defs.csv");

    // b1,3.0,5.0
    // b2,1.0,2.0
    const std::string basket_thresholds_file_path =
        xyz::test_utils::resolve_path("data/test/two_baskets_with_shared_ticker/basket_thresholds.csv");

    std::shared_ptr<xyz::app_context> app_context =
        xyz::app_context::read_files(basket_defs_file_path, basket_thresholds_file_path);

    // Smoke test the config to make sure it is reasonable.
    ASSERT_EQ(app_context->get_basket_config()->m_basket_name_to_entry_map.size(), 2);
    ASSERT_EQ(app_context->get_basket_config()->m_ticker_set.size(), 3);

    const std::shared_ptr<xyz::basket_manager> basket_manager = app_context->get_basket_manager();
    const std::shared_ptr<xyz::basket> b1_basket = basket_manager->get_basket("b1");
    const std::shared_ptr<xyz::basket> b2_basket = basket_manager->get_basket("b2");
    const std::shared_ptr<xyz::basket_monitor> basket_monitor = app_context->get_basket_monitor();
    const std::shared_ptr<xyz::market_data_service> mds = app_context->get_market_data_service();

    /////////////////////
    // last price
    //
    ASSERT_EQ(0, b1_basket->get_last_price_update_count());
    ASSERT_EQ(0, b1_basket->get_last_price());
    ASSERT_EQ(0, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(0, basket_monitor->get_last_price("b1"));

    ASSERT_EQ(0, b2_basket->get_last_price_update_count());
    ASSERT_EQ(0, b2_basket->get_last_price());
    ASSERT_EQ(0, basket_monitor->get_last_price_update_count("b2"));
    ASSERT_EQ(0, basket_monitor->get_last_price("b2"));

    mds->update_last_price("c2", 55);

    ASSERT_EQ(1, b1_basket->get_last_price_update_count());
    ASSERT_EQ(5500, b1_basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(5500, basket_monitor->get_last_price("b1"));

    ASSERT_EQ(0, b2_basket->get_last_price_update_count());
    ASSERT_EQ(0, b2_basket->get_last_price());
    ASSERT_EQ(0, basket_monitor->get_last_price_update_count("b2"));
    ASSERT_EQ(0, basket_monitor->get_last_price("b2"));

    mds->update_last_price("c1", 12);

    ASSERT_EQ(2, b1_basket->get_last_price_update_count());
    ASSERT_EQ(7900, b1_basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(7900, basket_monitor->get_last_price("b1"));

    ASSERT_EQ(1, b2_basket->get_last_price_update_count());
    ASSERT_EQ(600, b2_basket->get_last_price());
    ASSERT_EQ(1, basket_monitor->get_last_price_update_count("b2"));
    ASSERT_EQ(600, basket_monitor->get_last_price("b2"));

    mds->update_last_price("c3", 75, xyz::validate_ticker::yes);

    ASSERT_EQ(2, b1_basket->get_last_price_update_count());
    ASSERT_EQ(7900, b1_basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(7900, basket_monitor->get_last_price("b1"));

    ASSERT_EQ(2, b2_basket->get_last_price_update_count());
    ASSERT_EQ(2475, b2_basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b2"));
    ASSERT_EQ(2475, basket_monitor->get_last_price("b2"));

    // tiny price change will not trigger monitor threshold
    mds->update_last_price("c3", 75.01, xyz::validate_ticker::yes);

    ASSERT_EQ(2, b1_basket->get_last_price_update_count());
    ASSERT_EQ(7900, b1_basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b1"));
    ASSERT_EQ(7900, basket_monitor->get_last_price("b1"));

    ASSERT_EQ(3, b2_basket->get_last_price_update_count());
    ASSERT_EQ(2475.25, b2_basket->get_last_price());
    ASSERT_EQ(2, basket_monitor->get_last_price_update_count("b2"));
    ASSERT_EQ(2475, basket_monitor->get_last_price("b2"));

    /////////////////////
    // bid & ask prices
    //
    ASSERT_EQ(0, b1_basket->get_bid_price_update_count());
    ASSERT_EQ(0, b1_basket->get_bid_price());
    ASSERT_EQ(0, b1_basket->get_ask_price_update_count());
    ASSERT_EQ(0, b1_basket->get_ask_price());
    ASSERT_EQ(0, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(0, basket_monitor->get_mid_price("b1"));

    ASSERT_EQ(0, b2_basket->get_bid_price_update_count());
    ASSERT_EQ(0, b2_basket->get_bid_price());
    ASSERT_EQ(0, b2_basket->get_ask_price_update_count());
    ASSERT_EQ(0, b2_basket->get_ask_price());
    ASSERT_EQ(0, basket_monitor->get_mid_price_update_count("b2"));
    ASSERT_EQ(0, basket_monitor->get_mid_price("b2"));

    mds->update_bid_ask_prices("c1", 12, 13);

    ASSERT_EQ(1, b1_basket->get_bid_price_update_count());
    ASSERT_EQ(2400, b1_basket->get_bid_price());
    ASSERT_EQ(1, b1_basket->get_ask_price_update_count());
    ASSERT_EQ(2600, b1_basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    ASSERT_EQ(1, b2_basket->get_bid_price_update_count());
    ASSERT_EQ(600, b2_basket->get_bid_price());
    ASSERT_EQ(1, b2_basket->get_ask_price_update_count());
    ASSERT_EQ(650, b2_basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b2"));
    ASSERT_EQ(625, basket_monitor->get_mid_price("b2"));

    mds->update_bid_ask_prices("c3", 72, 73);

    ASSERT_EQ(1, b1_basket->get_bid_price_update_count());
    ASSERT_EQ(2400, b1_basket->get_bid_price());
    ASSERT_EQ(1, b1_basket->get_ask_price_update_count());
    ASSERT_EQ(2600, b1_basket->get_ask_price());
    ASSERT_EQ(1, basket_monitor->get_mid_price_update_count("b1"));
    ASSERT_EQ(2500, basket_monitor->get_mid_price("b1"));

    ASSERT_EQ(2, b2_basket->get_bid_price_update_count());
    ASSERT_EQ(2400, b2_basket->get_bid_price());
    ASSERT_EQ(2, b2_basket->get_ask_price_update_count());
    ASSERT_EQ(2475, b2_basket->get_ask_price());
    ASSERT_EQ(2, basket_monitor->get_mid_price_update_count("b2"));
    ASSERT_EQ(2437.5, basket_monitor->get_mid_price("b2"));
}
