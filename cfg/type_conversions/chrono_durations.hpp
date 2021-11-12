//
// Created by xzato on 12/11/21.
//

#include "cfg/option.hpp"

#include <chrono>

namespace cfg::type_conversion
{
    template<class OPTION_T>
    struct chrono
    {
        using value_t = value_t<OPTION_T>;

        static value_t convert_from_string(const std::string_view& input)
        {
            return value_t{std::stoi(std::string{input})};
        }

        static std::string convert_to_string(const value_t& input)
        {
            return std::to_string(input.count());
        }
    };
}

