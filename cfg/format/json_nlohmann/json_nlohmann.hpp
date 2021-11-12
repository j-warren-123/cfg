//
// Created by xzato on 17/08/21.
//

#pragma once

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "cfg/option.hpp"

namespace cfg::formats
{
    struct json_nlohmann
    {
    public:
        json_nlohmann() = delete;

    public:
        using data_type = nlohmann::json;

    public:
        template <class UNPARSED_TYPE>
        static data_type parse(const UNPARSED_TYPE& unparsed_json)
        {
            return nlohmann::json::parse(unparsed_json);
        }

        template <class VALUE_T>
        static void add(data_type& format_data,
                        const char* section_name,
                        const char* option_name,
                        const VALUE_T& value)
        {
            format_data[section_name][option_name] = value;
        }

        // either throws or the option exists
        static void exists(const data_type& format_data,
                           const char* section_name,
                           const char* option_name)
        {
            if (!format_data.contains(section_name))
                throw std::runtime_error{fmt::format("> missing [{}]\n", section_name)};

            // check the section contains the specified name
            if (!format_data[section_name].contains(option_name))
                throw std::runtime_error{
                    fmt::format("> [{}] missing [{}]\n", section_name, option_name)};
        }

        template <class OPTION_TYPE, class RETURN_TYPE = value_t<OPTION_TYPE>>
        static RETURN_TYPE get(const data_type& format_data,
                               const char* section_name,
                               OPTION_TYPE& option)
        {
            try
            {
                return format_data[section_name][OPTION_TYPE::name].template get<RETURN_TYPE>();
            }
            catch (std::exception& e)
            {
                throw std::runtime_error{fmt::format("> [{}]:[{}] parse failure with error: {}",
                                                     section_name,
                                                     OPTION_TYPE::name,
                                                     e.what())};
            }
        }

        static std::string string(const data_type& data) { return data.dump(4); }
    };
} // namespace cfg::formats
