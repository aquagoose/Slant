#include "Slant/Common.h"

const char* slResultToString(SlResult result)
{
#define SL_STR(Res) case SL_RESULT_##Res: return "SL_RESULT_"#Res;
    switch (result)
    {
        SL_STR(OK)
        SL_STR(UNKNOWN_ERROR)
        SL_STR(OUT_OF_MEMORY)
        SL_STR(INVALID_PARAMETER)
        SL_STR(INVALID_CONTEXT)
        SL_STR(INVALID_BUFFER)
        SL_STR(INVALID_SOURCE)
    }
#undef SL_STR
}
