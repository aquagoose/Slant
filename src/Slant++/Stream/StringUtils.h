#pragma once

#include <string>

namespace Slant
{
#ifdef _WIN32
    std::wstring ToWString(const std::string& string);
#endif
}