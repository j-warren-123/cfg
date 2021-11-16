#pragma once

//
// Created by xzato on 12/11/21.
//

#include <filesystem>

namespace cfg::type_conversion
{

    struct filesystem_path
    {
        static std::filesystem::path convert_from_string(const std::string_view& input)
        {
            return std::filesystem::path{input};
        }

        static std::string convert_to_string(const std::filesystem::path& input)
        {
            return input.string();
        }
    };
}

