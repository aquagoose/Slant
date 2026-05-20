#include "StringUtils.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace mixr
{
#ifdef _WIN32
    std::wstring ToWString(const std::string &string)
    {
        size_t length = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, 0, 0);
        std::wstring wstr(length, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, &wstr[0], static_cast<int>(wstr.length()));
        return wstr;
    }
#endif
}
