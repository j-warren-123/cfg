//
// Created by xzato on 21/08/21.
//

#pragma once

#include "cfg/helper/common.hpp"
#include "cfg/option.hpp"

namespace cfg
{
    template <class SECTION_T, class OPTIONS_T = std::tuple<>>
    class section : public SECTION_T
    {
    public:
        using SECTION_T::name;

    public:
        using section_t = SECTION_T;
        using options_t = OPTIONS_T;

    public:
        section() = default;

    public:
        template <option OPTION>
        OPTION& get()
        {
            return std::get<OPTION>(options);
        }

    public:
        OPTIONS_T options;
    };

    template <class SECTION>
    using section_t = typename SECTION::section_t;

    template <class USER_SECTION_T>
    struct section_builder : private USER_SECTION_T
    {
    private:
        using USER_SECTION_T::name;

    public:
        template <option... OPTIONS>
        struct add_options
        {
            using type = section<USER_SECTION_T, std::tuple<OPTIONS...>>;
        };
    };

} // namespace cfg