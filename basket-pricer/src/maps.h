//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_MAPS_H
#define BASKET_PRICER_MAPS_H

#include <unordered_set>
#include <vector>

namespace xyz {

    namespace maps {

        // static
        template <typename map_t>
        std::unordered_set<typename map_t::key_type>
        create_unordered_set_from_keys(const map_t& map) {

            std::unordered_set<typename map_t::key_type> set;
            auto end = map.end();
            for (auto iter = map.begin(); iter != end; ++iter) {

                set.emplace(iter->first);
            }
            return set;
        }

        // static
        template <typename map_t>
        std::vector<typename map_t::mapped_type>
        create_vector_from_values(const map_t& map) {

            std::vector<typename map_t::mapped_type> vec;
            auto end = map.end();
            for (auto iter = map.begin(); iter != end; ++iter) {

                vec.emplace_back(iter->second);
            }
            return vec;
        }
    }
}

#endif //BASKET_PRICER_MAPS_H
