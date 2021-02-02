
#include <iostream>
#include "app_context.h"

int main() {

    std::cout << "Wiring test begin" << std::endl;
    std::shared_ptr<xyz::app_context> app_context =
        xyz::app_context::read_files(
            "data/basket_defs.csv", "data/basket_thresholds.csv");
    std::cout << "Wiring test end" << std::endl;
    return 0;
}
