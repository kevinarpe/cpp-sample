//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_H
#define BASKET_PRICER_BASKET_H

#include <string>
#include <vector>
#include <functional>
#include "basket_entry.h"

namespace xyz {

    class basket {

    public:
        basket(std::string name, std::vector<basket_entry> entry_vec)
            : m_name(std::move(name)), m_entry_vec(std::move(entry_vec))
        {}

        typedef std::function<void(void)> price_update_callback_t;

        // Ex: "HSI"
        const std::string m_name;

        void set_price_update_callbacks(price_update_callback_t last_price, price_update_callback_t mid_price) {

            m_last_price_update_callback = std::move(last_price);
            m_mid_price_update_callback = std::move(mid_price);
        }

        void update_last_price(const int entry_index, const double ticker_price) {

            basket_entry& entry = m_entry_vec[entry_index];
            const double new_last_price_shares = ticker_price * entry.m_num_shares;
            if (new_last_price_shares != entry.m_mutable_last_price_shares) {

                const double last_diff = new_last_price_shares - entry.m_mutable_last_price_shares;
                entry.m_mutable_last_price_shares = new_last_price_shares;
                m_last_price += last_diff;
                ++m_last_price_update_count;
                m_last_price_update_callback();
            }
        }

        void update_bid_ask_prices(const int entry_index,
                                   const double ticker_bid_price,
                                   const double ticker_ask_price) {
            bool update = false;
            basket_entry& entry = m_entry_vec[entry_index];
            {
                const double new_bid_price_shares = ticker_bid_price * entry.m_num_shares;
                if (new_bid_price_shares != entry.m_mutable_bid_price_shares) {

                    const double bid_diff = new_bid_price_shares - entry.m_mutable_bid_price_shares;
                    entry.m_mutable_bid_price_shares = new_bid_price_shares;
                    m_bid_price += bid_diff;
                    ++m_bid_price_update_count;
                    update = true;
                }
            }
            {
                const double new_ask_price_shares = ticker_ask_price * entry.m_num_shares;
                if (new_ask_price_shares != entry.m_mutable_ask_price_shares) {
                    const double ask_diff = new_ask_price_shares - entry.m_mutable_ask_price_shares;
                    entry.m_mutable_ask_price_shares = new_ask_price_shares;
                    m_ask_price += ask_diff;
                    ++m_ask_price_update_count;
                    update = true;
                }
            }
            if (update) {
                m_mid_price_update_callback();
            }
        }

        [[nodiscard]]  // <-- recommended by Clang CTidy!
        double get_last_price()
        const {
            return m_last_price;
        }

        [[nodiscard]]
        int get_last_price_update_count()
        const {
            return m_last_price_update_count;
        }

        [[nodiscard]]
        double get_bid_price()
        const {
            return m_bid_price;
        }

        [[nodiscard]]
        int get_bid_price_update_count()
        const {
            return m_bid_price_update_count;
        }

        [[nodiscard]]
        double get_ask_price()
        const {
            return m_ask_price;
        }

        [[nodiscard]]
        int get_ask_price_update_count()
        const {
            return m_ask_price_update_count;
        }

        [[nodiscard]]
        double get_mid_price()
        const {
            const double x = (m_bid_price + m_ask_price) / 2.0;
            return x;
        }

    private:
        friend class basket_manager;
        std::vector<basket_entry> m_entry_vec;
        double m_last_price = 0;
        int m_last_price_update_count = 0;
        double m_bid_price = 0;
        int m_bid_price_update_count = 0;
        double m_ask_price = 0;
        int m_ask_price_update_count = 0;
        price_update_callback_t m_last_price_update_callback = nullptr;
        price_update_callback_t m_mid_price_update_callback = nullptr;
    };
}

#endif //BASKET_PRICER_BASKET_H
