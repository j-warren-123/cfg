# CFG

Header-only configuration file library.

- Generate the code to parse a config file using C++ templates.
- Statically check if option can be parsed (since the code is generated from templates, check if code exists to parse option)
- Suitable to be adapted for common formats (json implemented using nlohmann, can be expanded)
- Optional fields in the options (validation/include units etc.)

## Design Explanation

The basic premise of configuration files is that you have some string representation of the data, this is then converted into
user defined data types, which is then stored in some data structure which is passed around or global.

I wanted to combine all of these and get something where you:
1. Define your data structures for holding the options (sections and options)
2. Create "types" based on these data structures
3. Pass these types to a parser generator which can then generate code to parse a config file into the data structures
4. Add ability to retrieve data in a modification-safe way (if options change, code won't compile until its fixed).

## Using it

Start by setting up data structures to hold parsed data:

```c++
SECTION(section1);
SECTION(section2);

struct NAMED(option1)
{
    VALUE(std::string) = "lool option 1";
    const char* description = "desp option1";
};

struct NAMED(option2)
{
    VALUE(std::vector<int>) = {1, 2, 3};
    static constexpr const char* description = "desp option2";
    static constexpr const char* unit = "meters";
    static std::optional<std::string> validate(const std::vector<int>& v)
    {
        if (v != std::vector<int>{1,2,3})
            return "validation failed";
        
        return {};
    };
};
```

Notice the use of MACROS:

```c++
struct NAMED(option1) -> struct option1 : public option1_name // struct option1_name{ const char* name = "option1"; }
VALUE(std::vector<int>) = {1, 2, 3}; -> const char* value_type_name = "std::vector<int>"; std::vector<int> value = {1, 2, 3};
```

This is done as a convenience to provide some reflection but can be avoided, there are a number of fields which must be included
and the user is free to include them however they like.

Using the library:

```c++
int main()
{
    // only json is implemented atm, and uses nlohmann
    using namespace nlohmann;

    // create a test json string, this can come from a file or something else
    auto test_json =
        R"({
        "section1": {
            "option1": "lool option 1",
            "option2": [
                1,
                2,
                3
                ]
                },
                "section2": {
            "option1": "lool option 1",
            "option2": [
                81541,
                2,
                3
                ]
                },
                "section3": {
            "option1": "lool option 1"
        }
    })"_json;

    // add using for brevity
    using namespace cfg;

    // define the types of each section (notice ::type on the end, and using keyword)
    using sec1 = section_builder<section1>::add_options<option1, option2>::type;
    using sec2 = section_builder<section2>::add_options<option1, option2>::type;

    // create objects of these types and pass it to the parser generator (notice it 
    // is using json_nlohmann, using different formats will be as easy as swapping the template arg)
    auto pg = parser_generator<formats::json_nlohmann>(sec1{}, sec2{});

    // pg.verbose() provides a verbose string of the arguments
    std::cout << pg.verbose();
    
    //    {
    //        "section1": {
    //            "option1": "[unit: N/A] [type: std::string] - desp option1",
    //            "option2": "[unit: meters] [type: std::vector<int>] - desp option2"
    //        },
    //        "section2": {
    //            "option1": "[unit: N/A] [type: std::string] - desp option1",
    //            "option2": "[unit: meters] [type: std::vector<int>] - desp option2"
    //        }
    //    }
    
    
    // pg.string() provides a string of the current values, since these are defaulted from above, we get those
    std::cout << pg.string();
    
    //    {
    //        "section1": {
    //            "option1": "lool option 1",
    //            "option2": [
    //                1,
    //                2,
    //                3
    //            ]
    //        },
    //        "section2": {
    //            "option1": "lool option 1",
    //            "option2": [
    //                1,
    //                2,
    //                3
    //            ]
    //        }
    //    }    
    
    // pg.parse(test_json) will now parse the json string above - notice in the test string 
    // section2->option1 vector[0] is 81541
    pg.parse(test_json);
    
    // and print out again 
    std::cout << pg.string();
    //    {
    //        "section1": {
    //            "option1": "lool option 1",
    //            "option2": [
    //                1,
    //                2,
    //                3
    //            ]
    //        },
    //        "section2": {
    //            "option1": "lool option 1",
    //            "option2": [
    //                81541,
    //                2,
    //                3
    //            ]
    //        }
    //    }
    
    // get values from the pg, full type decl so you see its the same type as provided in the data structures
    std::string s1op1 = pg.get<section1>().get<option1>().value;
    std::vector<int>& s2op2 = pg.get<section2>().get<option2>().value;
    
    return 0;
}
```

Also, when parse was called, the pg will check if there is a "validate" function, and if there is, call it:

```c++
    // pg.parse(test_json);
    if (auto err = pg.parse(test_json))
        fmt::print(*err);

    // > validation failure for option: [option2], value: [[81541,2,3]], error:[validation failed]
```

And compile time checking if a variable is accessible:

```c++
    using sec1 = section_builder<section1>::add_options<option1, option2>::type;
    // using sec2 = section_builder<section2>::add_options<option1, option2>::type;
    using sec2 = section_builder<section2>::add_options<option1>::type; // option2 removed
```

```c++
    std::vector<int>& s2op2 = pg.get<section2>().get<option2>().value; // won't compile
```

## How to include

This uses C++20 (for concepts)

Clone repo and add using "add_subdirectory" to your project CMake:

> add_subdirectory(../cfg cfg)

> target_link_libraries(${PROJECT_NAME} PRIVATE cfg)

Since its header only, you can also just add cfg to your include path.

It uses nlohmann and fmt, if you haven't tried these libraries out yet, I'd recommend them both!

## Notes

I've used this as a way to learn concepts and decided I liked it enough to provide to others. I'll add ini file parsing
next I think and maybe others like yaml. I'm also open to adding new functionality to the parser generator (such as allowing
custom formatting since I'm already using fmt).
