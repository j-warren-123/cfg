//
// Created by xzato on 17/08/21.
//

#pragma once

namespace cfg
{

#ifndef SECTION
#define SECTION(A) struct A { constexpr static const char * name = #A; }
#endif

#ifndef NAMED
#define NAMED(the_class)                                                        \
the_class ## _name { public: constexpr static const char* name = #the_class; }; \
struct the_class : public the_class ## _name
#endif

#ifndef VALUE
#define VALUE(...)                                      \
constexpr static const char* value_type_name = #__VA_ARGS__; \
__VA_ARGS__ value
#endif

}
