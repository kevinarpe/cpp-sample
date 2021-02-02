//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_MONITOR_H
#define BASKET_PRICER_BASKET_MONITOR_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include "basket_config.h"
#include "basket.h"

namespace xyz {

    class basket_monitor {

    public:
        basket_monitor() = default;

        basket_monitor(const std::shared_ptr<xyz::basket_config>& basket_config,
                       std::vector<std::shared_ptr<basket>>& basket_vec) {

            for (std::shared_ptr<basket>& basket : basket_vec) {

                const basket_config::entry& basket_config_entry = basket_config->get(basket->m_name);
                entry_& entry = m_basket_name_to_entry_map[basket->m_name];  // Autovivification

                basket->set_price_update_callbacks(
                    [basket, &basket_config_entry, &entry]
                        () {
                        const double last_price = basket->get_last_price();
                        const double pct_diff = 100.0 * (last_price - entry.prev_last_price) / entry.prev_last_price;  // Note: if 0 == entry.prev_last_price, then pct_diff == 'inf'

                        if (pct_diff >= basket_config_entry.threshold.last_price_percent) {

                            log_(basket, "Last", entry.prev_last_price, last_price, pct_diff,
                                 basket_config_entry.threshold.last_price_percent);

                            entry.prev_last_price = last_price;
                            ++entry.last_price_update_count;
                        }
                    },
                    [basket, &basket_config_entry, &entry]
                        () {
                        const double mid_price = basket->get_mid_price();
                        const double pct_diff = 100.0 * (mid_price - entry.prev_mid_price) / entry.prev_mid_price;

                        if (pct_diff >= basket_config_entry.threshold.mid_price_percent) {

                            log_(basket, "Mid", entry.prev_mid_price, mid_price, pct_diff,
                                 basket_config_entry.threshold.mid_price_percent);

                            entry.prev_mid_price = mid_price;
                            ++entry.mid_price_update_count;
                        }
                    });
            }
        }

        double
        get_last_price(const std::string& basket_name)
        const {

            const entry_& entry = get_entry_(basket_name);
            return entry.prev_last_price;
        }

        double
        get_last_price_update_count(const std::string& basket_name)
        const {

            const entry_& entry = get_entry_(basket_name);
            return entry.last_price_update_count;
        }

        double
        get_mid_price(const std::string& basket_name)
        const {

            const entry_& entry = get_entry_(basket_name);
            return entry.prev_mid_price;
        }

        double
        get_mid_price_update_count(const std::string& basket_name)
        const {

            const entry_& entry = get_entry_(basket_name);
            return entry.mid_price_update_count;
        }

    private:
        struct entry_ {
            double prev_last_price = 0;
            int last_price_update_count = 0;
            double prev_mid_price = 0;
            int mid_price_update_count = 0;
        };
        std::unordered_map<std::string, entry_> m_basket_name_to_entry_map;

        const entry_&
        get_entry_(const std::string& basket_name) const {

            auto i = m_basket_name_to_entry_map.find(basket_name);
            if (m_basket_name_to_entry_map.end() == i) {
                throw std::invalid_argument(std::string("Unknown basket name: [").append(basket_name).append("]"));
            }
            const entry_& x = i->second;
            return x;
        }

        static
        void log_(const std::shared_ptr<basket>& basket, const std::string& price_type,
                  const double prev_price, const double price,
                  const double pct_diff, const double pct_diff_threshold) {

            std::cout << std::fixed << std::setprecision(2)
                      << "basket_monitor: Basket [" << basket->m_name << "]: " << price_type << " price: " << prev_price
                      << " -> " << price << ", diff: " << pct_diff << "%, threshold: " << pct_diff_threshold << "%\n";
        }
    };
}

#endif //BASKET_PRICER_BASKET_MONITOR_H
