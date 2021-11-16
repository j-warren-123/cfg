#pragma once

#include <cfg/option.hpp>

namespace cfg::type_conversion
{
    template<class CONTAINER>
    struct container
    {
        static std::string convert_to_string(const CONTAINER& cont)
        {
            static_assert(is_container<CONTAINER>::value);
            return fmt::format("{}", fmt::join(cont, ", "));
        }
    };

}