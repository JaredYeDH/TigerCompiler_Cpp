#pragma once

#include "common.h"

enum class ErrorCode : uint64_t
{
    Err0, Err1, Err2, Err3, Err4,
    Err5, Err6, Err7, Err8, Err9,
    Err10, Err11, Err12, Err13, Err14,
    Err15, Err16, Err17, Err18, Err19,
    Err20, Err21, Err22, Err23, Err24,
    Err25, Err26, Err27, Err28, Err29,
    Err30, Err31, Err32, Err33, Err34,
    Err35, Err36, Err37, Err38, Err39,
    Err40, Err41, Err42, Err43, Err44,
    Err45, Err46, Err47, Err48, Err49,
    Err50, Err51, Err52, Err53, Err54,
    Err55, Err56, Err57, Err58, Err59,
    Err60, Err61, Err62, Err63, Err64,
    Err65,

    CountErrors
}; 

namespace ErrorCodes
{
    const char* GetAssociatedMessage(ErrorCode errorCode);
}
