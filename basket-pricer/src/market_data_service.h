//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_MARKET_DATA_SERVICE_H
#define BASKET_PRICER_MARKET_DATA_SERVICE_H

#include <string>
#include <functional>
#include <unordered_set>
#include <memory>
#include "basket_manager.h"

namespace xyz {

    enum class validate_ticker { yes, no };

    class market_data_service {

    public:
        typedef std::function<void(const std::string& ticker, const double price)> update_last_price_callback_t;

        typedef std::function<void(const std::string& ticker, const double bid_price, const double ask_price)>
            update_bid_ask_prices_callback_t;

        static std::shared_ptr<market_data_service>
        subscribe(std::unordered_set<std::string> ticker_set,
                  std::shared_ptr<basket_manager> basket_manager) {

            const std::shared_ptr<market_data_service> x =
                std::make_shared<market_data_service>(
                    std::move(ticker_set),
                    [basket_manager]
                    (const std::string& ticker, const double price) {
                        basket_manager->update_last_price(ticker, price);
                    },
                    [basket_manager]
                    (const std::string& ticker, const double bid_price, const double ask_price) {
                        basket_manager->update_bid_ask_prices(ticker, bid_price, ask_price);
                    });
            return x;
        }

        market_data_service() = default;

        market_data_service(std::unordered_set<std::string> ticker_set,
                            update_last_price_callback_t update_last_price_callback,
                            update_bid_ask_prices_callback_t update_bid_ask_prices_callback)
            : m_ticker_set(std::move(ticker_set)),
              m_update_last_price_callback(std::move(update_last_price_callback)),
              m_update_bid_ask_prices_callback(std::move(update_bid_ask_prices_callback))
        {}

        void update_last_price(const std::string& ticker, const double price,
                               validate_ticker validate_ticker = validate_ticker::yes) {

            if (! is_valid_ticker_(ticker, validate_ticker)) {
                return;
            }
            m_update_last_price_callback(ticker, price);
        }

        void update_bid_ask_prices(const std::string& ticker,
                                   const double bid_price, const double ask_price,
                                   validate_ticker validate_ticker = validate_ticker::yes) {

            if (! is_valid_ticker_(ticker, validate_ticker)) {
                return;
            }
            m_update_bid_ask_prices_callback(ticker, bid_price, ask_price);
        }

    private:
        std::unordered_set<std::string> m_ticker_set;
        update_last_price_callback_t m_update_last_price_callback;
        update_bid_ask_prices_callback_t m_update_bid_ask_prices_callback;

        bool is_valid_ticker_(const std::string& ticker, validate_ticker validate_ticker) {

            // make it easy to "mock" -- send dummy data
            if (validate_ticker::yes == validate_ticker) {
                const bool x = (m_ticker_set.end() != m_ticker_set.find(ticker));
                return x;
            }
            else {
                return true;
            }
        }
    };
}

#endif //BASKET_PRICER_MARKET_DATA_SERVICE_H
