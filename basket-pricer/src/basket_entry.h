//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_ENTRY_H
#define BASKET_PRICER_BASKET_ENTRY_H

#include <string>

namespace xyz {

    class basket_entry {

    public:
        // Required for std::vector
        basket_entry() = default;

        basket_entry(std::string ticker, const double num_shares)
            : m_ticker(std::move(ticker)), m_num_shares(num_shares)
            {}

    private:
        friend class basket;
        friend class basket_manager;
        // Ex: "0005.HK"
        std::string m_ticker;
        double m_num_shares;
        // last_price * shares
        double m_mutable_last_price_shares = 0;
        // bid_price * shares
        double m_mutable_bid_price_shares = 0;
        // ask_price * shares
        double m_mutable_ask_price_shares = 0;
    };
}

#endif //BASKET_PRICER_BASKET_ENTRY_H
