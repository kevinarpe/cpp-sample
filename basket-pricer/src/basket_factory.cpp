//
// Created by kca on 17/8/2019.
//

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "basket_factory.h"
#include "basket_entry.h"
#include "maps.h"

namespace xyz {

    // static
    std::vector<std::shared_ptr<basket>>
    basket_factory::
    create(const std::shared_ptr<xyz::basket_config>& basket_config) {

        std::map<std::string, std::map<std::string, basket_entry>> basket_name_to_ticker_to_basket_entry_map;

        for (auto iter = basket_config->m_basket_name_to_entry_map.begin();
             iter != basket_config->m_basket_name_to_entry_map.end();
             ++iter) {

            const std::string& basket_name = iter->first;
            const basket_config::entry& entry = iter->second;

            std::map<std::string, basket_entry>& ticker_to_basket_entry_map =
                basket_name_to_ticker_to_basket_entry_map[basket_name];  // Autovivification

            for (const basket_config::basket_def_entry& bde : entry.basket_def_entry_vec) {

                if (ticker_to_basket_entry_map.end() != ticker_to_basket_entry_map.find(bde.ticker)) {
                    throw std::logic_error(
                        std::string("basket [").append(basket_name).append("]: Duplicate ticker: [")
                            .append(bde.ticker).append("]"));
                }
                ticker_to_basket_entry_map.emplace(bde.ticker, basket_entry(bde.ticker, bde.num_shares));
            }
        }
        std::vector<std::shared_ptr<basket>> basket_vec;
        for (auto iter = basket_name_to_ticker_to_basket_entry_map.begin();
             iter != basket_name_to_ticker_to_basket_entry_map.end();
             ++iter) {

            const std::string& basket_name = iter->first;
            std::map<std::string, basket_entry>& ticker_to_basket_entry_map = iter->second;

            std::vector<basket_entry> basket_entry_vec = xyz::maps::create_vector_from_values(ticker_to_basket_entry_map);
            basket_vec.emplace_back(std::make_shared<basket>(basket_name, basket_entry_vec));
        }
        return basket_vec;
    }
}
