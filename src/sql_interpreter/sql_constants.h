#pragma once

#include <string_view>

namespace SQLConstants
{
    namespace Action
    {
        constexpr std::string_view CREATE = "CREATE";
        constexpr std::string_view DROP = "DROP";
    }

    namespace ObjectType
    {
        constexpr std::string_view DATABASE = "DATABASE";
        constexpr std::string_view TABLE = "TABLE";
    }

}