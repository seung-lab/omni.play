#pragma once

#include "pipeline/stage.h"


namespace om {
namespace pipeline {

class compress : stage<char, char>
{
private:
    char* compressed_;
    int outSize_;

public:
    compress(out_stage<char> pred)
        : stage<char, char>(pred)
    { }

    char* operator(char*);

    inline void cleanup() {
        delete [] compressed_;
    }

    inline int out_size() {
        return outSize_;
    }
};

}
}
