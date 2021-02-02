//
// Created by kca on 17/8/2019.
//

#ifndef BASKET_PRICER_BASKET_FACTORY_H
#define BASKET_PRICER_BASKET_FACTORY_H

#include <string>
#include <vector>
#include <memory>
#include "basket_config.h"
#include "basket_factory.h"
#include "basket.h"

namespace xyz {

    class basket_factory {

    public:
        static std::vector<std::shared_ptr<basket>>
        create(const std::shared_ptr<xyz::basket_config>& basket_config);
    };
}

#endif //BASKET_PRICER_BASKET_FACTORY_H
