#pragma once

#include <string>

namespace mixr
{
#ifdef _WIN32
    std::wstring ToWString(const std::string& string);
#endif
}