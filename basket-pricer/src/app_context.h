//
// Created by kca on 18/8/2019.
//

#ifndef BASKET_PRICER_APP_CONTEXT_H
#define BASKET_PRICER_APP_CONTEXT_H

#include <string>
#include <utility>
#include <memory>
#include "basket_config.h"
#include "basket_factory.h"
#include "basket.h"
#include "basket_manager.h"
#include "basket_monitor.h"
#include "market_data_service.h"

namespace xyz {

    class app_context {
    public:
        static
        std::shared_ptr<app_context>
        read_files(const std::string& basket_defs_file_path, const std::string& basket_thresholds_file_path) {

            const std::shared_ptr<basket_config> basket_config =
                basket_config::read_files(basket_defs_file_path, basket_thresholds_file_path);
            // TODO: better: return std::set where basket overloads operator< correctly.
            std::vector<std::shared_ptr<basket>> basket_vec = basket_factory::create(basket_config);
            std::shared_ptr<basket_manager> basket_manager = std::make_shared<xyz::basket_manager>(basket_vec);
            std::shared_ptr<basket_monitor> basket_monitor =
                std::make_shared<xyz::basket_monitor>(basket_config, basket_vec);
            std::shared_ptr<market_data_service> market_data_service =
                market_data_service::subscribe(basket_config->m_ticker_set, basket_manager);

            const std::shared_ptr<app_context> x =
                std::make_shared<app_context>(
                    basket_config, basket_vec, basket_manager, basket_monitor, market_data_service);
            return x;
        }
        // required for Google Test
        app_context() = default;

        app_context(std::shared_ptr<basket_config> basket_config,
                    std::vector<std::shared_ptr<basket>> basket_vec,
                    std::shared_ptr<basket_manager> basket_manager,
                    std::shared_ptr<basket_monitor> basket_monitor,
                    std::shared_ptr<market_data_service> market_data_service)
            : m_basket_config(std::move(basket_config)),
              m_basket_vec(std::move(basket_vec)),
              m_basket_manager(std::move(basket_manager)),
              m_basket_monitor(std::move(basket_monitor)),
              m_market_data_service(std::move(market_data_service))
            {}

        [[nodiscard]]
        std::shared_ptr<basket_config>
        get_basket_config()
        const {
            return m_basket_config;
        }

        [[nodiscard]]
        std::shared_ptr<basket_manager>
        get_basket_manager()
        const {
            return m_basket_manager;
        }

        [[nodiscard]]
        std::shared_ptr<basket_monitor>
        get_basket_monitor()
        const {
            return m_basket_monitor;
        }

        [[nodiscard]]
        std::shared_ptr<market_data_service>
        get_market_data_service() {
            return m_market_data_service;
        }
    private:
        std::shared_ptr<basket_config> m_basket_config;
        std::vector<std::shared_ptr<basket>> m_basket_vec;
        std::shared_ptr<basket_manager> m_basket_manager;
        std::shared_ptr<basket_monitor> m_basket_monitor;
        std::shared_ptr<market_data_service> m_market_data_service;
    };
}

#endif //BASKET_PRICER_APP_CONTEXT_H
