#pragma once

#include <string_view>

namespace InputConstants
{
    namespace Action
    {
        constexpr std::string_view PUT = "PUT";

        constexpr std::string_view DELETE = "DELETE";
        constexpr std::string_view CLEAR = "CLEAR"; // i.e. delete all

        constexpr std::string_view GET = "GET";
        constexpr std::string_view LIST = "LIST"; // i.e. list all

        constexpr std::string_view EXIT = "EXIT";
    }
}