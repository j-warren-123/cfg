//
// Created by xzato on 17/08/21.
//

#pragma once

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include "cfg/helper/common.hpp"
#include "cfg/option.hpp"

namespace cfg::formats
{
    class json_nlohmann
    {
    public:
        using data_type = nlohmann::json;

    public:
        template <class OPTION_VALUE_TYPE>
        static void add(nlohmann::json& format_data,
                        const char* section_name,
                        const char* option_name,
                        const OPTION_VALUE_TYPE& value)
        {
            format_data[section_name][option_name] = value;
        }

        template <option OPTION_TYPE>
        static std::string get(const nlohmann::json& format_data,
                               const char* section_name,
                               OPTION_TYPE& option)
        {

            if (!format_data.contains(section_name))
                return fmt::format("> missing [{}]\n", section_name);

            // check the section contains the specified name
            if (!format_data[section_name].contains(option.name))
                return fmt::format("> [{}] missing [{}]\n", section_name, option.name);

            try
            {
                option.value =
                    format_data[section_name][option.name].template get<value_t<OPTION_TYPE>>();
            }
            catch (std::exception& ex)
            {
                return fmt::format("> type error: [{}], for [{}][{}]\n",
                                   ex.what(),
                                   section_name,
                                   option.name);
            }

            if (auto resp = validate<std::remove_reference_t<OPTION_TYPE>>(option.value))
                return fmt::format(
                    "> validation failure for option: [{}], value: [{}], error:[{}]\n",
                    option.name,
                    format_data[section_name][option.name].dump(),
                    *resp);

            return {};
        }

        static std::string string(const data_type& data) { return data.dump(4); }
    };
} // namespace cfg::formats
