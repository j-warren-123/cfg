//
// Created by xzato on 11/11/21.
//

#pragma once

#include "cfg/option.hpp"

#include <fmt/core.h>
#include <string>
#include <tuple>

namespace cfg
{
    template <class... SECTIONS>
    class configuration
    {
    public:
        using sections_type = std::tuple<SECTIONS...>;

    public:
        template <class SECTION>
        const SECTION& get_section()
        {
            return std::get<typename SECTION::parent_section_t>(_sections);
        }

        template <class SECTION, class OPTION>
        const value_t<OPTION>& get_value_from()
        {
            return get_section<SECTION>().template get_value_from<OPTION>();
        }

    public:
        template <class SECTION_FUNCTION, class OPTION_FUNCTION>
        void for_each(const SECTION_FUNCTION& to_section, const OPTION_FUNCTION& to_option)
        {
            auto apply_to_option = [&to_option](auto& section_obj, auto& option_obj) {
                to_option(section_obj, option_obj);
            };

            auto apply_to_section = [&to_section, &apply_to_option](auto& section_obj) {
                to_section(section_obj);
                std::apply(
                    [&apply_to_option, &section_obj](auto&... option_obj) {
                        (apply_to_option(section_obj, option_obj), ...);
                    },
                    section_obj.options);
            };

            std::apply(
                [&apply_to_section](auto&... section_obj) { (apply_to_section(section_obj), ...); },
                _sections);
        }

        template <class OPTION_FUNCTION>
        void for_each(const OPTION_FUNCTION& to_option)
        {
            auto apply_to_section = [&to_option](auto& section_obj) {
                std::apply([&to_option, &section_obj](
                               auto&... option_obj) { (to_option(section_obj, option_obj), ...); },
                           section_obj.options);
            };

            std::apply([&apply_to_section](
                           auto&... section_obj) { ((apply_to_section(section_obj)), ...); },
                       _sections);
        }

    public:
        // print the config options using the user provided format string, use:
        // - {0} to get section
        // - {1} for option name
        // - {2} for option value
        // - {3} for option value type
        // - {4} for option description
        // - {5} for option unit
        std::string get_options(std::string_view section_format_string = "{0}\n",
                                std::string_view option_format_string = "\t{1} = {2}\n")
        {
            std::string formatted_data;
            for_each(

                [&formatted_data, &section_format_string](const auto& section_obj) {
                    formatted_data +=
                        fmt::format(section_format_string,
                                    std::remove_reference_t<decltype(section_obj)>::name);
                },

                [&formatted_data, &option_format_string](const auto& section_obj,
                                                         const auto& option_obj) {
                    // get the type of the option_obj
                    using option_type = std::remove_reference_t<decltype(option_obj)>;
                    using section_type = std::remove_reference_t<decltype(section_obj)>;

                    if constexpr (has_convert_to_string<option_type>::value)
                    {
                        formatted_data += fmt::format(option_format_string,
                                                      section_type::name,
                                                      option_type::name,
                                                      option_type::convert_to_string(option_obj.value),
                                                      option_type::value_type_name,
                                                      option_type::description,
                                                      get_unit<option_type>());
                    }
                    else
                    {
                        formatted_data += fmt::format(option_format_string,
                                                      section_type::name,
                                                      option_type::name,
                                                      option_obj.value,
                                                      option_type::value_type_name,
                                                      option_type::description,
                                                      get_unit<option_type>());
                    }


                });
            return formatted_data;
        }

        std::string get_verbose_options()
        {
            return get_options("[{}]\n",
                               "  *  {1}: "
                               "[default: {2}] "
                               "[unit: {5}] "
                               "[type: {3}]\n"
                               "       -- {4}\n");
        }

    private:
        sections_type _sections;
    };
} // namespace cfg
