//
// Created by xzato on 17/08/21.
//

#pragma once

#include "cfg/section.hpp"

#include <fmt/core.h>

#include <iostream>
#include <tuple>

namespace cfg
{
    template <class FORMAT>
    struct parser
    {
    public:
        // the parser doesn't contain state, you're not supposed to create one
        parser() = delete;

    public:
        using format_data_t = typename FORMAT::data_type;

    public:
        template <class CONFIGURATION_T, class INPUT_DATA_T>
        static CONFIGURATION_T parse(const INPUT_DATA_T& input_data)
        {
            format_data_t format_data = FORMAT::parse(input_data);
            CONFIGURATION_T returned_config; // initially empty

            std::string errors;

            returned_config.for_each([&errors,
                                      &format_data,
                                      &returned_config](const auto& section_obj, auto& option_obj) {
                // get the type of the option_obj
                using option_type = std::remove_reference_t<decltype(option_obj)>;
                using section_type = std::remove_reference_t<decltype(section_obj)>;

                try
                {
                    FORMAT::exists(format_data, section_type::name, option_type::name);
                }
                catch (std::exception& ex)
                {
                    errors += fmt::format("{}\n", ex.what());
                    return;
                }

                value_t<option_type> parsed_option_value;
                try
                {
                    // TODO: add ability to parse UD types
                    parsed_option_value = FORMAT::get(format_data, section_type::name, option_obj);
                }
                catch (std::exception& ex)
                {
                    errors += fmt::format("{}\n", ex.what());
                    return;
                }

                // if there is a validate function, then validate it
                if constexpr (has_validate<option_type>::value)
                {
                    try
                    {
                        option_type::validate(parsed_option_value);
                    }
                    catch (std::exception& ex)
                    {
                        errors += fmt::format(
                            "> [{}]:[{}] validation failure with value: [{}], error: {}\n",
                            section_type::name,
                            option_type::name,
                            format_data[section_type::name][option_type::name].dump(),
                            ex.what());
                        return;
                    }
                }

                // and if all these pass, then move the parsed value into the option value field
                option_obj.value = std::move(parsed_option_value);
            });

            if (!errors.empty())
                throw std::runtime_error(fmt::format("\n{}", errors));

            return returned_config;
        }

        template <class CONFIGURATION>
        static std::string generate_example_config(const CONFIGURATION& config = CONFIGURATION{})
        {
            format_data_t format_data;

            config.for_each([&config, &format_data](const auto& section_obj,
                                                    const auto& option_obj) {
                // get the type of the option_obj
                using option_type = std::remove_reference_t<decltype(option_obj)>;
                using section_type = std::remove_reference_t<decltype(section_obj)>;

                // TODO: allow convert to string on UD types
                FORMAT::add(format_data, section_type::name, option_type::name, option_obj.value);
            });

            return FORMAT::string(format_data);
        }
    };
} // namespace cfg
