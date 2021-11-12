# CFG

Header-only configuration file library.

- Generate the code to parse common text-based configuration files (.ini/xml/json/yaml) using C++ templates.
- Statically check if option can be parsed (since the code is generated from templates, check if code exists to parse
  option)
- Optional fields in the options (validation/include units etc.)

## Design Explanation

Configuration parsing typically is done by defining a struct/class and then reading some file to populate the fields in
the struct/class. The code to parse the file and fill the correct fields is boilerplate and can be generated. There are
tools that allow you to do this outside of C++ using programs to generate the C++ code that is then included.

The goal of this library is to do something similar, except, the code generation is done with templates using standard
C++17 (except fmt for string output) within the program. The benefit is that if you have a standard compilant compiler
for a given platform, the code generation is available easily (no need to worry about getting the code generation
program compiled/installed for that platform). Therefore, the goals of the library are to:

1. Define C++ data structures to hold the configuration options in classes/structs
2. The structures are types, therefore are compile-time accessible for template metaprogramming. This allows for:
    1. Generation of parsing code based on the configuration data structures
    2. Type checking on access, access with type as template parameter instead of const char* so code doesn't compile
       if the option doesn't exist
3. Parsing code generation is simple to change, it can adapt to another text-based format without modifying data 
   structures (e.g. json/XML)

Therefore, this library provides tools for creating the configuration as a set of types, then allowed you to pass those
types to other structures to parse configuration files.

## Using it

### Create Options

This is intrusive in that you are required that the options have specific fields which are used by the parser generator. 
I have included MACROS to make getting these fields easier, such as the name of the option as a const char* to reflect
the type name.

The option can be separate from the section its in, so it can be reused in multiple sections if required.

```c++
#include "cfg/helper/reflection.hpp"
#include <string>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};
```

[Note that it is possible to add extract fields to the option as required, so long as the fields for the parser generator are there]

### Create Sections

Compose each section as a combination of the name of the section and the options inside it

```c++
#include "cfg/helper/reflection.hpp"
#include "cfg/section.hpp"

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

struct section1 : public cfg::section_base<section1, option1>
{
    constexpr static const char* name = "section1";
};

SECTION(section2, option1, option2){};
```

[Note that it is possible to add extract fields to the section as required, so long as the fields for the parser generator are there]

### Create the Configuration

Compose the configuration as a combination of the sections, we can create a type from this or instantiate an object from
it

```c++
#include "cfg/helper/reflection.hpp"
#include "cfg/section.hpp"
#include <cfg/configuration.hpp>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

struct section1 : public cfg::section_base<section1, option1>
{
    constexpr static const char* name = "section1";
};

SECTION(section2, option1, option2){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;
    auto config = cfg::configuration<section1, section2>();
};
```

### Accessing the fields in a Configuration:

```c++
#include "cfg/helper/reflection.hpp"
#include "cfg/section.hpp"
#include <cfg/configuration.hpp>

#include <iostream>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

SECTION(section1, option1){};
SECTION(section2, option1, option2){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;
    auto config = cfg::configuration<section1, section2>();

    std::cout << config.get_section<section1>().get_option<option1>().description << std::endl; // desp option1
    std::string s = config.get_section<section1>().get_option<option1>().value;
};
```

1 of 2 situations can arise, the option is renamed/removed entirely, or the option is removed from a specific section.
1. If the option is removed or renamed, then the class doesn't exist and you cannot use it as a template param.
2. The option is not in the section also fails to compile:

```c++
#include "cfg/helper/reflection.hpp"
#include "cfg/section.hpp"
#include <cfg/configuration.hpp>

#include <iostream>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

SECTION(section1, option1){};
SECTION(section2, option2){}; // <- option 1 removed

int main()
{
    using config_t = cfg::configuration<section1, section2>;
    auto config = cfg::configuration<section1, section2>();

    std::string s = config.get_section<section2>().get_option<option1>().value; // fails to compile
};
```

### Generate Example Configuration string

Since the configuration is defined by the C++ data structures, it makes sense to include a way of creating a 
configuration string that can be parsed back again. 

```c++
#include "cfg/configuration.hpp"
#include "cfg/helper/reflection.hpp"
#include "cfg/parser.hpp"

#include "cfg/format/json_nlohmann/json_nlohmann.hpp"

#include <iostream>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

struct section1 : public cfg::section_base<section1, option1>
{
    constexpr static const char* name = "section1";
};

SECTION(section2, option1, option2){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;
    auto config = cfg::configuration<section1, section2>();

    std::cout << cfg::parser<cfg::formats::json_nlohmann>::generate_example_config<config_t>()
              << std::endl;
    
    /*
    {
        "section1": {
            "option1": "option 1 value"
        },
        "section2": {
            "option1": "option 1 value",
            "option2": "option 2"
        }
    }
     */
};
```

### Parsing

Now take the output from the generate function and feed it in a string to parse, making sure to change one of the 
option values (for example save the generate_config to a file, then edit the file and read it back in).

```c++
#include "cfg/configuration.hpp"
#include "cfg/helper/reflection.hpp"
#include "cfg/parser.hpp"

#include "cfg/format/json_nlohmann/json_nlohmann.hpp"

#include <iostream>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

SECTION(section1, option1){};
SECTION(section2, option1, option2){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;

    // notice the value of section 2 option 1 has changed:
    const char* test_string = R"(
    {
        "section1": {
            "option1": "option 1 value"
        },
        "section2": {
            "option1": "NEW option 1 value",
            "option2": "option 2"
        }
    })";

    auto config = cfg::parser<cfg::formats::json_nlohmann>::parse<config_t>(test_string);
    std::cout << config.get_section<section1>().get_option<option1>().value << std::endl; // option 1 value
    std::cout << config.get_section<section2>().get_option<option1>().value << std::endl; // NEW option 1 value
};
```

### Optional Stuff

The library will use specific functions in the options if they're accessible at compile time as a priority, otherwise
it will defer that work to the backing library or do nothing. If the backing library doesn't support the functionality, 
then it will most likely fail to compile or error in the same way the library would.

#### User defined conversions

If you want your type to be, for instance, a std::chrono::duration, e.g. std::chrono::seconds, then you can define a 
conversion which accepts a string and returns a std::chrono::seconds, and vice versa. It is not strictly necessary to
define both, it depends which functionality is required. For instance, it is sufficient to only define a conversion for
the type to a string to generate a configuration string:

```c++

#include "cfg/configuration.hpp"
#include "cfg/helper/reflection.hpp"
#include "cfg/parser.hpp"
#include "cfg/section.hpp"

#include "cfg/format/json_nlohmann/json_nlohmann.hpp"

#include <chrono>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

OPTION(option3)
{
    VALUE(std::chrono::seconds){10};
    static constexpr const char* description = "desp option2";

    // if this is commented out, then generate_example_config will attempt to use the backing library conversion for
    // std::chrono::seconds to string, nlohmann doesn't have one so this doesn't compile
    static std::string convert_to_string(const decltype(value)& value)
    {
        return std::to_string(value.count());
    }
};

SECTION(section1, option1){};
SECTION(section2, option1, option2, option3){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;
    auto config = cfg::configuration<section1, section2>();

    std::cout << cfg::parser<cfg::formats::json_nlohmann>::generate_example_config<config_t>()
              << std::endl;
};

```

Since the above code doesn't provide a way to convert a string to a std::chrono::seconds, we cannot use parse, however
if this functionality is added, then parse can be used:

```c++
#include "cfg/configuration.hpp"
#include "cfg/helper/reflection.hpp"
#include "cfg/parser.hpp"
#include "cfg/section.hpp"

#include "cfg/format/json_nlohmann/json_nlohmann.hpp"

#include <chrono>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

OPTION(option3)
{
    VALUE(std::chrono::seconds){10};
    static constexpr const char* description = "desp option2";

    static std::string convert_to_string(const decltype(value)& value)
    {
        return std::to_string(value.count());
    }

    static decltype(value) convert_from_string(const std::string_view& str)
    {
        return decltype(value){std::stoi(std::string{str})};
    }
};

SECTION(section1, option1){};
SECTION(section2, option1, option2, option3){};

int main()
{
    using config_t = cfg::configuration<section1, section2>;

    auto configuration_string =
        cfg::parser<cfg::formats::json_nlohmann>::generate_example_config<config_t>();
    auto config = cfg::parser<cfg::formats::json_nlohmann>::parse<config_t>(configuration_string);
};
```

#### Value Validation

It's sometimes useful to limit the range of values a specific option can take:

```c++
#include "cfg/configuration.hpp"
#include "cfg/helper/reflection.hpp"
#include "cfg/parser.hpp"
#include "cfg/section.hpp"

#include "cfg/format/json_nlohmann/json_nlohmann.hpp"

#include <chrono>

struct option1
{
    constexpr static const char* name = "option1";
    constexpr static const char* value_type_name = "std::string";
    std::string value = "option 1 value";
    static constexpr const char* description = "desp option1";
};

OPTION(option2)
{
    VALUE(std::string) = "option 2";
    static constexpr const char* description = "desp option2";
};

OPTION(option3)
{
    VALUE(std::chrono::seconds){10};
    static constexpr const char* description = "desp option2";

    static std::string convert_to_string(const decltype(value)& value)
    {
        return std::to_string(value.count());
    }

    static decltype(value) convert_from_string(const std::string_view& str)
    {
        return decltype(value){std::stoi(std::string{str})};
    }
};

OPTION(option4)
{
    VALUE(int) = 2; // notice this is set to 2, we only allow 1 or 0
    static constexpr const char* description = "Only 1 or 0";

    static void validate(const decltype(value)& val)
    {
        if (!(val == 1 || val == 0))
            throw std::runtime_error("option 4 can only be 1 or 0");
    }
};

SECTION(section1, option1){};
SECTION(section2, option1, option2, option3){};
SECTION(section3, option4){};

int main()
{
    using config_t = cfg::configuration<section1, section2, section3>;

    auto configuration_string =
        cfg::parser<cfg::formats::json_nlohmann>::generate_example_config<config_t>();

    try
    {
        // try and parse the config, but the value is incorrect
        auto config = cfg::parser<cfg::formats::json_nlohmann>::parse<config_t>(configuration_string);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        // > [section3]:[option4] validation failure with value: [2], error: option 4 can only be 1 or 0
    }
};
```

There is some other "optional" functionality, check option.hpp to see what else can be added. This may expand in the 
future.

## How to include

This uses C++17 and fmt.

Clone repo and add using "add_subdirectory" to your project CMake:

```cmake
add_subdirectory(**insert path to cfg**/cfg cfg)
target_link_libraries(${PROJECT_NAME} PRIVATE cfg)
```

Since the heavy work of parsing a specific library is done using a backing library, then you need to make sure the
corresponding library is available, and the corresponding option has been added to the cmake generate. Check the 
CMakeLists.txt to see the option names, e.g.:

```cmake
option(CFG_USE_JSON_NLOHMANN "Use the nlohmann json library" True)
if (${CFG_USE_JSON_NLOHMANN})
add_subdirectory(cfg/format/json_nlohmann)
target_link_libraries(${PROJECT_NAME} INTERFACE cfg_json_nlohmann)
endif()
```

so to use json parsing with the nlohmann backing library, use:

```
-DCFG_USE_JSON_NLOHMANN=True
```

Alongside the other flags.

