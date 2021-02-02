
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <random>
#include "app_context.h"

template <typename duration_t>
std::chrono::steady_clock::time_point
log_duration(const std::chrono::steady_clock::time_point begin, const char* const units, const char* const desc,
             int64_t* duration) {

    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    *duration = std::chrono::duration_cast<duration_t>(end - begin).count();
    std::cout << desc << ": " << *duration << units << " elapsed" << std::endl;
    return end;
}

int get_per_ticket_update_count(const int argc, const char* const* const argv) {

    if (1 == argc) {
        return 1000;
    }
    else {
        const int x = std::stoi(argv[1]);
        if (x <= 0) {
            throw std::invalid_argument(
                std::string("Invalid command line argument 'per_ticket_update_count': [").append(argv[1])
                .append("]"));
        }
        return x;
    }
}

int main(const int argc, const char* const* const argv) {

    const int per_ticket_update_count = get_per_ticket_update_count(argc, argv);

    std::cout << "Throughput test begin" << std::endl;
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::chrono::time_point last = begin;
    std::shared_ptr<xyz::app_context> app_context =
        xyz::app_context::read_files(
            "../data/test/throughput/basket_defs.csv",
            "../data/test/throughput/basket_thresholds.csv");

    int64_t duration;
    last = log_duration<std::chrono::milliseconds>(last, "ms", "init done", &duration);

    const int ticker_count = app_context->get_basket_config()->m_ticker_set.size();
    std::vector<std::string> ticker_vec(ticker_count);
    std::copy(
        std::begin(app_context->get_basket_config()->m_ticker_set),
        std::end(app_context->get_basket_config()->m_ticker_set),
        std::begin(ticker_vec));

    std::vector<int> ticker_index_vec(ticker_count);
    std::iota(std::begin(ticker_index_vec), std::end(ticker_index_vec), 0);
    auto rng = std::default_random_engine {};

    std::vector<int> full_ticker_index_vec;
    full_ticker_index_vec.reserve(ticker_count * per_ticket_update_count);

    for (int cycle = 1; cycle <= per_ticket_update_count; ++cycle) {

        std::shuffle(std::begin(ticker_index_vec), std::end(ticker_index_vec), rng);
        std::copy(
            std::begin(ticker_index_vec),
            std::end(ticker_index_vec),
            std::back_inserter(full_ticker_index_vec));
    }
    std::shared_ptr<xyz::market_data_service> p_mds = app_context->get_market_data_service();
    // Avoid overhead of operator*()
    xyz::market_data_service& mds = *p_mds;

    const size_t size = full_ticker_index_vec.size();
    std::cout << "Update count: " << size << std::endl;

    last = log_duration<std::chrono::milliseconds>(last, "ms", "begin test", &duration);
    for (size_t i = 0; i < size; ++i) {

        const int ticker_index = full_ticker_index_vec[i];
        const std::string& ticker = ticker_vec[ticker_index];
        const double bid_price = 1.0 + (((double) i) / 100.0);
        mds.update_bid_ask_prices(ticker, bid_price, 1.0 + bid_price);
    }
    last = log_duration<std::chrono::microseconds>(last, "µs", "end test", &duration);
    const double micro_per_update = ((double) duration) / ((double) size);
    std::cout << "Update count: " << size << " @ "
              << std::fixed << std::setprecision(2)
              << micro_per_update << "µs per update" << std::endl;
    std::cout << "Throughput test end" << std::endl;
    return 0;
}
