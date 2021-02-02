//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_CONFIG_H
#define BASKET_PRICER_BASKET_CONFIG_H

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>

namespace xyz {

    class basket_config {

    public:
        static std::shared_ptr<basket_config>
        read_files(const std::string& basket_defs_file_path, const std::string& basket_thresholds_file_path);

        static const std::string EXCEPTION_MESSAGE;

        struct basket_def_entry {
            std::string ticker;
            double num_shares;

            basket_def_entry(std::string  ticker, double num_shares)
            : ticker(std::move(ticker)), num_shares(num_shares)
            {}
        };
        struct Threshold {
            double last_price_percent;
            double mid_price_percent;

            // required for std::vector
            // see: basket_config.cpp: basket_name_to_threshold_map[basket_name]
            Threshold() = default;

            Threshold(double last_price_percent, double mid_price_percent)
            : last_price_percent(last_price_percent), mid_price_percent(mid_price_percent)
            {}
        };
        struct entry {
            std::vector<basket_def_entry> basket_def_entry_vec;
            Threshold threshold;

            entry(std::vector<basket_def_entry> basket_def_entry_vec, Threshold threshold)
            : basket_def_entry_vec(std::move(basket_def_entry_vec)), threshold(threshold)
            {}
        };

        basket_config() = default;

        explicit
        basket_config(std::map<std::string, entry> basket_name_to_entry_map)
            : m_basket_name_to_entry_map(std::move(basket_name_to_entry_map)) {

            for (const auto& iter : m_basket_name_to_entry_map) {

                for (const basket_def_entry& entry : iter.second.basket_def_entry_vec) {

                    m_ticker_set.emplace(entry.ticker);
                }
            }
        }

        const entry&
        get(const std::string& basket_name)
        const {

            auto i = m_basket_name_to_entry_map.find(basket_name);
            if (m_basket_name_to_entry_map.end() == i) {
                throw std::invalid_argument(std::string("Unknown basket: [").append(basket_name).append("]"));
            }
            const entry& x = i->second;
            return x;
        }

        std::map<std::string, entry> m_basket_name_to_entry_map;
        std::unordered_set<std::string> m_ticker_set;
    };
}

#endif //BASKET_PRICER_BASKET_CONFIG_H
