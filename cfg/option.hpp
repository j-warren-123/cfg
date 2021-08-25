//
// Created by xzato on 17/08/21.
//

#pragma once

#include "helper/common.hpp"

#include <concepts>
#include <optional>

namespace cfg
{
    // clang-format off

    // the required fields for a valid option
    template <class OPTION_T>
    concept option = named<OPTION_T> && requires(OPTION_T t)
    {
        {t.value};
        {OPTION_T::value_type_name} -> std::convertible_to<const char*>;
        {OPTION_T::description} -> std::convertible_to<const char*>;
    };

    // helpful in defining the value_type of the value in an option
    template<option OPTION_T>
    using value_t = decltype(std::declval<OPTION_T>().value);

    template <class OPTION_T>
    concept has_unit = requires()
    {
        {OPTION_T::unit} -> std::convertible_to<const char*>;
    };

    template <class OPTION_T>
    concept has_validate = option<OPTION_T> && requires(value_t<OPTION_T> v)
    {
        {OPTION_T::validate(v)} -> std::convertible_to<error_msg>;
    };
    // clang-format on

    // if the option has a unit field, then use that
    template <has_unit T>
    constexpr const char* get_unit()
    {
        return T::unit;
    }

    // otherwise use N/A
    template <class T>
    constexpr const char* get_unit()
    {
        return "N/A";
    }

    // if the option has a validate function taking the type of the value field, use that.
    // the validate function returns a error message is something went wrong
    template <has_validate OPTION>
    error_msg validate(const value_t<OPTION>& v)
    {
        return OPTION::validate(v);
    }

    // so if there is no validate function, then we can just return an empty error message
    // i.e. no error
    template <class OPTION>
    error_msg validate(const value_t<OPTION>& v)
    {
        return {};
    }

} // namespace cfg
