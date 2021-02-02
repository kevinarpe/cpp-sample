//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_MANAGER_H
#define BASKET_PRICER_BASKET_MANAGER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include "basket.h"
#include "basket_entry.h"
#include "basket_config.h"

namespace xyz {

    class basket_manager {

    public:
        basket_manager() = default;

        explicit
        basket_manager(std::vector<std::shared_ptr<basket>>& basket_vec) {

            for (std::shared_ptr<basket>& basket : basket_vec) {
                {
                    auto i = m_basket_name_to_basket_map.find(basket->m_name);
                    if (m_basket_name_to_basket_map.end() != i) {
                        throw std::invalid_argument(
                            std::string("Duplicate basket name: [").append(basket->m_name).append("]"));
                    }
                    m_basket_name_to_basket_map.emplace(basket->m_name, basket);
                }
                const int size = basket->m_entry_vec.size();
                for (int i = 0; i < size; ++i) {

                    basket_entry& entry = basket->m_entry_vec[i];
                    std::vector<entry_>& entry_vec = m_ticker_to_entry_vec_map[entry.m_ticker];  // Autovivification
                    entry_vec.emplace_back(entry.m_ticker, basket, i);
                }
            }
        }

        void update_last_price(const std::string& ticker, const double price) {
            // @Nullable
            std::vector<entry_>* entry_vec = try_get_entry_vec_(ticker);
            if (nullptr == entry_vec) {
                return;
            }
            if (! is_price_valid_(ticker, price, LAST_PRICE_TYPE)) {
                return;
            }
            for (entry_& entry : *entry_vec) {

                entry.m_basket->update_last_price(entry.m_entry_index, price);
            }
        }

        void update_bid_ask_prices(const std::string& ticker, const double bid_price, const double ask_price) {
            // @Nullable
            std::vector<entry_>* entry_vec = try_get_entry_vec_(ticker);
            if (nullptr == entry_vec) {
                return;
            }
            if (! are_bid_ask_prices_valid_(ticker, bid_price, ask_price)) {
                return;
            }
            for (entry_& entry : *entry_vec) {

                entry.m_basket->update_bid_ask_prices(entry.m_entry_index, bid_price, ask_price);
            }
        }

        std::shared_ptr<basket>
        get_basket(const std::string& basket_name)
        const {

            auto i = m_basket_name_to_basket_map.find(basket_name);
            if (m_basket_name_to_basket_map.end() == i) {
                throw std::invalid_argument(std::string("Unknown basket name: [").append(basket_name).append("]"));
            }
            const std::shared_ptr<basket>& x = i->second;
            return x;
        }
    private:
        static const std::string LAST_PRICE_TYPE;
        static const std::string BID_PRICE_TYPE;
        static const std::string ASK_PRICE_TYPE;
        std::unordered_map<std::string, std::shared_ptr<basket>> m_basket_name_to_basket_map;

        struct entry_ {
            // @Debug
            std::string m_ticker;
            std::shared_ptr<basket> m_basket;
            int m_entry_index;

            entry_(std::string ticker, std::shared_ptr<basket> basket, const int entry_index)
                : m_ticker(std::move(ticker)), m_basket(std::move(basket)), m_entry_index(entry_index)
                {}
        };
        std::unordered_map<std::string, std::vector<entry_>> m_ticker_to_entry_vec_map;

        // @Nullable
        std::vector<entry_>*
        try_get_entry_vec_(const std::string& ticker) {

            auto iter = m_ticker_to_entry_vec_map.find(ticker);
            if (iter == m_ticker_to_entry_vec_map.end()) {

                std::cout << "basket_manager: Unknown ticker: [" << ticker << "]" << std::endl;
                return nullptr;
            }
            std::vector<entry_>& x = iter->second;
            return &x;
        }

        static
        bool is_price_valid_(const std::string& ticker, const double price, const std::string& price_type) {
            // prefer most frequent code path: valid price
            if (price > 0.0) {
                return true;
            }
            else {
                std::cout << "basket_manager: Ticker [" << ticker << "]: " << price_type << " price is invalid: "
                          << price << std::endl;
                return false;
            }
        }

        static
        bool are_bid_ask_prices_valid_(const std::string& ticker, const double bid_price, const double ask_price) {
            // weird logic here to prefer most frequent code path: valid prices
            if (! is_price_valid_(ticker, bid_price, BID_PRICE_TYPE)
                    || ! is_price_valid_(ticker, ask_price, ASK_PRICE_TYPE)) {

                return false;
            }
            else if (bid_price < ask_price) {
                return true;
            }
            else {
                std::cout << "basket_manager: Ticker [" << ticker << "]: Bid and ask prices are crossed: "
                          << bid_price << " vs " << ask_price << std::endl;
                return false;
            }
        }
    };
}

#endif //BASKET_PRICER_BASKET_MANAGER_H
