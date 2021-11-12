//
// Created by xzato on 21/08/21.
//

#pragma once

#include "cfg/option.hpp"

namespace cfg
{
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
            return std::get<OPTION>(options);
        }

    public:
        options_t options;
    };
} // namespace cfg