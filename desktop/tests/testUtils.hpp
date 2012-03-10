#pragma once

#include "common/omException.h"

//TODO: replace w/ ZI_VERIFY
inline void verify(const bool result)
{
    if(!result){
        throw OmVerifyException("verify failed");
    }
}

inline void verify0(const bool result)
{
    if(result){
        throw OmVerifyException("verify 0 failed");
    }
}

