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
        : std::is_invocable_r<void, decltype(&T::validate), const value_t<T>&>
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
        : std::is_invocable_r<std::string, decltype(&T::convert_to_string), value_t<T>>
    { };

    template <class T, typename = void>
    struct has_convert_from_string : std::false_type
    { };

    template <class T>
    struct has_convert_from_string<T, std::void_t<decltype(&T::convert_from_string)>>
        : std::is_invocable_r<value_t<T>, decltype(&T::convert_from_string), std::string_view>
    { };

    template <typename T>
    struct has_const_iterator
    {
    private:
        typedef char yes;
        typedef struct
        {
            char array[2];
        } no;

        template <typename C>
        static yes test(typename C::const_iterator*);
        template <typename C>
        static no test(...);

    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
        typedef T type;
    };

    template <typename T>
    struct has_begin_end
    {
        template <typename C>
        static char (&f(
            typename std::enable_if<std::is_same<decltype(static_cast<typename C::const_iterator (
                                                              C::*)() const>(&C::begin)),
                                                 typename C::const_iterator (C::*)() const>::value,
                                    void>::type*))[1];

        template <typename C>
        static char (&f(...))[2];

        template <typename C>
        static char (&g(
            typename std::enable_if<std::is_same<decltype(static_cast<typename C::const_iterator (
                                                              C::*)() const>(&C::end)),
                                                 typename C::const_iterator (C::*)() const>::value,
                                    void>::type*))[1];

        template <typename C>
        static char (&g(...))[2];

        static bool const beg_value = sizeof(f<T>(0)) == 1;
        static bool const end_value = sizeof(g<T>(0)) == 1;
    };

    template <typename T>
    struct is_container
        : std::integral_constant<bool,
                                 has_const_iterator<T>::value && has_begin_end<T>::beg_value &&
                                     has_begin_end<T>::end_value>
    { };

} // namespace cfg
