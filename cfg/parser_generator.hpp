//
// Created by xzato on 17/08/21.
//

#pragma once

#include "cfg/helper/common.hpp"
#include "cfg/option.hpp"
#include "cfg/section.hpp"

#include <fmt/core.h>
#include <tuple>

namespace cfg
{
    template <class FORMAT, class... SECTIONS>
    class parser
    {
    public:
        using sections_type = std::tuple<SECTIONS...>;

        using format_data_t = typename FORMAT::data_type;

    public:
        explicit parser(SECTIONS... sections) : _sections{sections...} { }

    public:

        error_msg parse(const format_data_t& format_data)
        {
            std::string errors; // initially empty

            std::apply(
                [&format_data, &errors](auto&... section) {
                    (std::apply(
                        [&format_data, &errors, section_name = section.name](auto&... option) {
                            ((errors += FORMAT::get(format_data, section_name, option)),
                              ...);
                         },
                         section.options),
                     ...);
                },
                _sections);

            if (!errors.empty())
                return errors;

            return {};
        }

        std::string verbose()
        {
            format_data_t format_data;
            std::apply(
                [&format_data](const auto&... elem) {
                    (std::apply(
                         [&format_data, section_name = elem.name](const auto&... elem) {
                             ((FORMAT::add(format_data,
                                           section_name,
                                           elem.name,
                                           fmt::format("[unit: {}] [type: {}] - {}",
                                                       get_unit<std::remove_reference<decltype(elem)>::type>(),
                                                       elem.value_type_name,
                                                       elem.description))),
                              ...);
                         },
                         elem.options),
                     ...);
                },
                _sections);
            return FORMAT::string(format_data);
        }

        std::string string()
        {
            format_data_t format_data;
            std::apply(
                [&format_data](const auto&... elem) {
                    (std::apply(
                         [&format_data, section_name = elem.name](const auto&... elem) {
                             ((FORMAT::add(format_data, section_name, elem.name, elem.value)), ...);
                         },
                         elem.options),
                     ...);
                },
                _sections);
            return FORMAT::string(format_data);
        }

    public:
        // returns the section provided. This either succeeds or tries to index a tuple with -1
        template <class SECTION>
        auto& get()
        {
            return std::get<get_section<SECTION, sections_type>()>(_sections);
        }

    private:
        sections_type _sections;

    private:
        // Get the index of the element with the section type as SECTION
        template <class SECTION,
                  class TUPLE_T,
                  size_t INDEX = 0,
                  size_t SIZE = std::tuple_size_v<std::remove_reference_t<TUPLE_T>>>
        constexpr static int get_section()
        {
            if constexpr (INDEX < SIZE)
            {
                if constexpr (std::is_same_v<section_t<std::tuple_element_t<INDEX, TUPLE_T>>,
                                             SECTION>)
                {
                    return INDEX;
                }

                return get_section<SECTION, TUPLE_T, INDEX + 1>();
            }
        }
    };

    template <class FORMAT, class... SECTIONS>
    parser<FORMAT, SECTIONS...> parser_generator(SECTIONS... sections)
    {
        return parser<FORMAT, SECTIONS...>{sections...};
    };
} // namespace cfg
