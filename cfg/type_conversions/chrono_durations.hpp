#pragma once

//
// Created by xzato on 12/11/21.
//

#include "cfg/option.hpp"

#include <chrono>

namespace cfg::type_conversion
{
    template<class VALUE_TYPE>
    struct chrono
    {

        static VALUE_TYPE convert_from_string(const std::string_view& input)
        {
            return VALUE_TYPE{std::stoi(std::string{input})};
        }

        static std::string convert_to_string(const VALUE_TYPE& input)
        {
            return std::to_string(input.count());
        }
    };
}

