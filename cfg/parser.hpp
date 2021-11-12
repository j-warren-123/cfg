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

            returned_config.for_each([&format_data, &returned_config](const auto& section_obj,
                                                                      auto& option_obj) {
                // get the type of the option_obj
                using option_type = std::remove_reference_t<decltype(option_obj)>;
                using section_type = std::remove_reference_t<decltype(section_obj)>;

                // check if the field exists using the FORMAT
                FORMAT::exists(format_data, section_type::name, option_type::name);

                value_t<option_type> parsed_option_value;
                // if there is a convert function, then use that
                if constexpr (has_convert_from_string<option_type>::value)
                {
                    // if it does, parse it and return it as the type of the option value
                    parsed_option_value = option_type::convert_from_string(
                        FORMAT::template get<option_type, std::string>(format_data,
                                                                       section_type::name,
                                                                       option_obj));
                }
                // otherwise, default to the underlying library to do the conversion
                else
                {
                    parsed_option_value = FORMAT::get(format_data, section_type::name, option_obj);
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
                        throw std::runtime_error{fmt::format(
                            "> [{}]:[{}] validation failure with value: [{}], error: {}\n",
                            section_type::name,
                            option_type::name,
                            format_data[section_type::name][option_type::name].dump(),
                            ex.what())};
                    }
                }

                // and if all these pass, then move the parsed value into the option value field
                option_obj.value = std::move(parsed_option_value);
            });

            return returned_config;
        }

        template <class CONFIGURATION>
        static std::string generate_example_config()
        {
            format_data_t format_data;
            CONFIGURATION temp_config;

            temp_config.for_each(
                [&temp_config, &format_data](const auto& section_obj, auto& option_obj) {
                    // get the type of the option_obj
                    using option_type = std::remove_reference_t<decltype(option_obj)>;
                    using section_type = std::remove_reference_t<decltype(section_obj)>;

                    // if there is a convert function, then use that
                    if constexpr (has_convert_to_string<option_type>::value)
                    {
                        FORMAT::add(format_data,
                                    std::remove_reference_t<decltype(section_obj)>::name,
                                    std::remove_reference_t<decltype(option_obj)>::name,
                                    option_type::convert_to_string(option_obj.value));
                    }
                    // otherwise, default to the underlying library to do the conversion
                    else
                    {
                        FORMAT::add(format_data,
                                    std::remove_reference_t<decltype(section_obj)>::name,
                                    std::remove_reference_t<decltype(option_obj)>::name,
                                    option_obj.value);
                    }
                });

            return FORMAT::string(format_data);
        }
    };
} // namespace cfg
