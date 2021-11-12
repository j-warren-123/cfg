//
// Created by xzato on 17/08/21.
//

#pragma once

#include <string_view>
#include <utility>

namespace cfg
{
    // helpful in defining the value_type of the value in an option
    template <class OPTION>
    using value_t = decltype(std::declval<OPTION>().value);

    template <class T, typename = void>
    struct has_unit : std::false_type
    { };

    template <class T>
    struct has_unit<T, std::void_t<decltype(std::declval<T>().unit)>>
        : std::is_same<decltype(std::declval<T>().unit), const char*>
    { };

    template <class T>
    constexpr const char* get_unit()
    {
        if constexpr (has_unit<T>::value)
            return std::remove_reference_t<T>::unit;
        return "N/A";
    }

    template <class T, typename = void>
    struct has_validate : std::false_type
    { };

    template <class T>
    struct has_validate<T, std::void_t<decltype(&T::validate)>>
        : std::is_invocable_r<void, decltype(&T::validate), decltype(std::declval<T>().value)>
    { };

    template <class T>
    void validate(const value_t<T>& value)
    {
        if constexpr (has_validate<T>::value)
            T::validate(value);
    }

    template <class T, typename = void>
    struct has_convert_to_string : std::false_type
    { };

    template <class T>
    struct has_convert_to_string<T, std::void_t<decltype(&T::convert_to_string)>>
        : std::is_invocable_r<std::string,
                              decltype(&T::convert_to_string),
                              decltype(std::declval<T>().value)>
    { };

    template <class T, typename = void>
    struct has_convert_from_string : std::false_type
    { };

    template <class T>
    struct has_convert_from_string<T, std::void_t<decltype(&T::convert_from_string)>>
        : std::is_invocable_r<decltype(std::declval<T>().value),
                              decltype(&T::convert_from_string),
                              std::string_view>
    { };

} // namespace cfg
