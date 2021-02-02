project(basket_pricer_lib)

add_library(basket_pricer_lib
        STATIC
        basket_factory.cpp basket_factory.h
        basket.h
        basket_entry.h
        basket_manager.cpp basket_manager.h
        string_trim.h
        basket_config.cpp basket_config.h
        basket_monitor.h
        market_data_service.h
        maps.h
        app_context.h)
