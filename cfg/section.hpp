//
// Created by xzato on 21/08/21.
//

#pragma once

#include "cfg/option.hpp"

#include <tuple>
#include <type_traits>
#include <chrono>
namespace cfg
{
    template <typename T, typename Tuple>
    struct has_type;

    template <typename T>
    struct has_type<T, std::tuple<>> : std::false_type
    { };

    template <typename T, typename U, typename... Ts>
    struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>>
    { };

    template <typename T, typename... Ts>
    struct has_type<T, std::tuple<T, Ts...>> : std::true_type
    { };

    // sections can inherit from this, or define the get_option function
    template <class SECTION, class... OPTIONS>
    class section_base
    {
    public:
        using parent_section_t = SECTION;
        using options_t = std::tuple<OPTIONS...>;

    public:
        section_base() = default;

    public:
        template <class OPTION>
        const OPTION& get_option() const
        {
            static_assert(has_type<OPTION, options_t>::value, "option not found in section");
            return std::get<OPTION>(options);
        }

        template <class OPTION>
        OPTION& get_option()
        {
            static_assert(has_type<OPTION, options_t>::value, "option not found in section");
            return std::get<OPTION>(options);
        }

        template <class OPTION>
        const value_t<OPTION>& get_value_from() const
        {
            static_assert(has_type<OPTION, options_t>::value, "option not found in section");
            return std::get<OPTION>(options).value;
        }

        // useful for if you want to set the outside of option struct
        template <class OPTION>
        void set_value(const value_t<OPTION>& input_value)
        {
            static_assert(has_type<OPTION, options_t>::value, "option not found in section");
            std::get<OPTION>(options).value = input_value;
        }

    public:
        options_t options;
    };
} // namespace cfg