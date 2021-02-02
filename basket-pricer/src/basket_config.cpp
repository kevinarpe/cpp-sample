//
// Created by kca on 17/8/2019.
//

#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include "basket_config.h"
#include "basket_entry.h"
#include "string_trim.h"
#include "basket.h"
#include "maps.h"

namespace xyz {

    // static
    const std::string basket_config::EXCEPTION_MESSAGE = "Basket names do not match between config files";

    namespace {

        double parse_positive_double_(const int line_num, const std::string& value, const std::string& arg_name) {

            std::size_t pos = 0;
            const double d = std::stod(value, &pos);
            if (value.size() != pos) {
                throw std::invalid_argument(std::string("Line #").append(std::to_string(line_num)).append(": ")
                                                .append("Failed to parse string as double: [").append(value).append("]"));
            }
            return d;
        }

        void
        append_token_(std::vector<std::string>& token_vec, const int line_num, std::string token) {

            string_trim::trim(token);
            if (token.empty()) {
                throw std::logic_error(
                    std::string("Line #").append(std::to_string(line_num)).append(": ")
                        .append("Token #").append(std::to_string(1 + token_vec.size()))
                        .append(" is empty"));
            }
            token_vec.emplace_back(token);
        }

        std::vector<std::string>
        split_by_delim_to_vec_(const int line_num, const std::string& line, const std::string& delim) {

            // Ref: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c#comment44856986_14266139
            // size_t last = 0; size_t next = 0; while ((next = s.find(delimiter, last)) != string::npos) { cout << s.substr(last, next-last) << endl; last = next + 1; } cout << s.substr(last) << endl;
            size_t last = 0;
            size_t next = 0;
            std::vector<std::string> token_vec;
            while (std::string::npos != (next = line.find(delim, last))) {

                std::string token = line.substr(last, next - last);
                append_token_(token_vec, line_num, token);
                last = 1 + next;
            }
            std::string token = line.substr(last);
            append_token_(token_vec, line_num, token);
            return token_vec;
        }

        bool try_parse_line_(const int line_num, std::string line,
                             std::string *basket_name, std::string *ticker, double *num_shares) {

            string_trim::trim(line);
            if (line.empty() || '#' == line[0]) {
                return false;
            }
            std::vector<std::string> token_vec = split_by_delim_to_vec_(line_num, line, ",");
            if (3 != token_vec.size()) {
                throw std::logic_error(
                    std::string("Line #").append(std::to_string(line_num)).append(": ")
                        .append("Expected three tokens, but found ")
                        .append(std::to_string(token_vec.size())));
            }
            *basket_name = token_vec[0];
            *ticker = token_vec[1];

            const std::string& num_shares_str = token_vec[2];
            *num_shares = parse_positive_double_(line_num, num_shares_str, "num_shares");

            return true;
        }

        std::map<std::string, std::vector<basket_config::basket_def_entry>>
        read_basket_defs_file_(const std::string& basket_defs_file_path) {

            std::map<std::string, std::map<std::string, basket_config::basket_def_entry>>
                basket_name_to_ticker_to_basket_def_entry_map;

            std::cout << "basket_config: Read config file: [" << basket_defs_file_path << "]" << std::endl;
            std::ifstream fin(basket_defs_file_path);
            if (! fin) {
                throw std::logic_error("Failed to open file for reading: [" + basket_defs_file_path + "]");
            }
            std::string line;
            int line_num = 0;
            while (std::getline(fin, line)) {

                ++line_num;

                std::string basket_name;
                std::string ticker;
                double num_shares;
                if (! try_parse_line_(line_num, line, &basket_name, &ticker, &num_shares)) {
                    continue;
                }
                std::map<std::string, basket_config::basket_def_entry>& ticker_to_basket_def_entry_map =
                    basket_name_to_ticker_to_basket_def_entry_map[basket_name];  // Autovivification

                if (ticker_to_basket_def_entry_map.end() != ticker_to_basket_def_entry_map.find(ticker)) {
                    throw std::logic_error(
                        std::string("Basket [").append(basket_name).append("]: Duplicate ticker: [")
                            .append(ticker).append("]"));
                }
                ticker_to_basket_def_entry_map.emplace(ticker, basket_config::basket_def_entry(ticker, num_shares));
            }
            std::map<std::string, std::vector<basket_config::basket_def_entry>> basket_name_to_basket_def_entry_vec_map;
            for (auto iter = basket_name_to_ticker_to_basket_def_entry_map.begin();
                 iter != basket_name_to_ticker_to_basket_def_entry_map.end();
                 ++iter) {

                const std::string& basket_name = iter->first;
                const std::map<std::string, basket_config::basket_def_entry>& ticker_to_basket_def_entry_map =
                    iter->second;

                std::vector<basket_config::basket_def_entry> basket_def_entry_vec;
                for (auto iter2 = ticker_to_basket_def_entry_map.begin();
                     iter2 != ticker_to_basket_def_entry_map.end();
                     ++iter2) {

                    basket_def_entry_vec.emplace_back(iter2->second);
                }
                basket_name_to_basket_def_entry_vec_map.emplace(basket_name, basket_def_entry_vec);
            }

            return basket_name_to_basket_def_entry_vec_map;
        }

        bool try_parse_line2_(const int line_num, std::string line,
                              std::string *basket_name,
                              double *last_price_percent_threshold,
                              double *mid_price_percent_threshold) {

            string_trim::trim(line);
            if (line.empty() || '#' == line[0]) {
                return false;
            }
            std::vector<std::string> token_vec = split_by_delim_to_vec_(line_num, line, ",");
            if (3 != token_vec.size()) {
                throw std::logic_error(
                    std::string("Line #").append(std::to_string(line_num)).append(": ")
                        .append("Expected three tokens, but found ")
                        .append(std::to_string(token_vec.size())));
            }
            *basket_name = token_vec[0];

            const std::string& last_price_percent_threshold_str = token_vec[1];
            *last_price_percent_threshold =
                parse_positive_double_(
                    line_num, last_price_percent_threshold_str, "last_price_percent_threshold");

            const std::string& mid_price_percent_threshold_str = token_vec[2];
            *mid_price_percent_threshold =
                parse_positive_double_(
                    line_num, mid_price_percent_threshold_str, "mid_price_percent_threshold");

            return true;
        }

        std::map<std::string, basket_config::Threshold>
        read_basket_thresholds_file_(const std::string& basket_thresholds_file_path) {

            std::map<std::string, basket_config::Threshold> basket_name_to_threshold_map;

            std::cout << "basket_config: Read config file: [" << basket_thresholds_file_path << "]" << std::endl;
            std::ifstream fin(basket_thresholds_file_path);
            if (! fin) {
                throw std::logic_error("Failed to open file for reading: [" + basket_thresholds_file_path + "]");
            }
            std::string line;
            int line_num = 0;
            while (std::getline(fin, line)) {

                ++line_num;

                std::string basket_name;
                double last_price_percent_threshold;
                double mid_price_percent_threshold;
                if (! try_parse_line2_(line_num, line,
                                       &basket_name, &last_price_percent_threshold, &mid_price_percent_threshold)) {
                    continue;
                }
                if (basket_name_to_threshold_map.end() != basket_name_to_threshold_map.find(basket_name)) {
                    throw std::logic_error(std::string("Duplicate basket [").append(basket_name).append("]"));
                }
                basket_name_to_threshold_map.emplace(
                    basket_name, basket_config::Threshold(last_price_percent_threshold, mid_price_percent_threshold));
            }
            return basket_name_to_threshold_map;
        }
    }

    // static
    std::shared_ptr<basket_config>
    basket_config::
    read_files(const std::string& basket_defs_file_path, const std::string& basket_thresholds_file_path) {

        std::map<std::string, std::vector<basket_config::basket_def_entry>> basket_name_to_basket_def_entry_vec_map =
            read_basket_defs_file_(basket_defs_file_path);

        const std::unordered_set<std::string>& basket_name_set1 =
            maps::create_unordered_set_from_keys(basket_name_to_basket_def_entry_vec_map);

        std::map<std::string, basket_config::Threshold> basket_name_to_threshold_map =
            read_basket_thresholds_file_(basket_thresholds_file_path);

        const std::unordered_set<std::string>& basket_name_set2 =
            maps::create_unordered_set_from_keys(basket_name_to_threshold_map);

        if (basket_name_set1 != basket_name_set2) {
            // TODO(arpeke): Improve this terrible error message!
            throw std::invalid_argument(EXCEPTION_MESSAGE);
        }
        std::map<std::string, basket_config::entry> basket_name_to_entry_map;
        for (const std::string& basket_name : basket_name_set1) {

            basket_name_to_entry_map.emplace(
                basket_name,
                basket_config::entry(
                    basket_name_to_basket_def_entry_vec_map[basket_name],
                    basket_name_to_threshold_map[basket_name]));
        }
        const std::shared_ptr<basket_config> x = std::make_shared<basket_config>(basket_name_to_entry_map);
        return x;
    }
}
