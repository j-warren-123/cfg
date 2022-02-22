//
// Created by xzato on 17/08/21.
//

#pragma once

namespace cfg
{

#ifndef NAMED
#define NAMED(the_class)                                                                           \
    struct the_class##_name                                                                        \
    {                                                                                              \
        constexpr static const char* name = #the_class;                                            \
    };                                                                                             \
    struct the_class : public the_class##_name
#endif

#ifndef OPTION
#define OPTION(the_class) NAMED(the_class)
#endif


#ifndef VALUE
#define VALUE(...)                                                                                 \
    constexpr static const char* value_type_name = #__VA_ARGS__;                                   \
    __VA_ARGS__ value
#endif

#ifndef DESCRIPTION
#define DESCRIPTION constexpr static const char* description
#endif

#ifndef SECTION
#define SECTION(SECTION_NAME, ...)                                                                 \
    NAMED(SECTION_NAME), public cfg::section_base<SECTION_NAME, __VA_ARGS__>
#endif

} // namespace cfg
