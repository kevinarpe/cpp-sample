//
// Created by kca on 18/8/2019.
//

#ifndef BASKET_PRICER_MAIN_TEST_UTILS_H
#define BASKET_PRICER_MAIN_TEST_UTILS_H

#include <string>
#include <filesystem>
#include <iostream>

namespace xyz {

    namespace test_utils {

        // Ref: https://stackoverflow.com/a/42772814/257299
        static
        std::string
        resolve_path(const std::string& relative_path) {

            const std::filesystem::path cwd = std::filesystem::current_path();
            std::cout << "test_utils::resolve_path(): cwd: [" << cwd.string() << "]" << std::endl;
            std::filesystem::path base_dir = cwd;
            while (true) {

                const std::filesystem::path& path = base_dir / relative_path;
                if (std::filesystem::exists(path)) {
                    const std::string& x = path.string();
                    std::cout << "test_utils::resolve_path(): [" << relative_path << "]->[" << x << "]" << std::endl;
                    return x;
                }
                if (! base_dir.has_parent_path()) {
                    break;
                }
                base_dir = base_dir.parent_path();
            }
            throw std::runtime_error(
                std::string("test_utils::resolve_path(): Failed to find relative path [").append(relative_path)
                    .append("] from current working directory: [").append(cwd.string()).append("]"));
        }
    }
}

#endif //BASKET_PRICER_MAIN_TEST_UTILS_H
