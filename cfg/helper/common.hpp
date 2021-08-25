//
// Created by xzato on 21/08/21.
//

#pragma once

#include <optional>

namespace cfg
{
    using error_msg = std::optional<std::string>;

    // clang-format off
    template <class OPTION_T>
    concept named = requires()
    {
        {OPTION_T::name} -> std::convertible_to<const char*>;
    };
    // clang-format on

} // namespace cfg
