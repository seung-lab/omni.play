#pragma once

#include "pipeline/stage.hpp"
#include <fstream>

namespace om {
namespace pipeline {

class write_out : public stage
{
private:
    std::string fnp_;

public:
    write_out(std::string fnp)
        : fnp_(fnp)
    { }

    template<typename T>
    data_var operator()(const data<T>& d) const
    {
        std::ofstream fout(fnp_.c_str());

        for(int i = 0; i < d.size; i++) {
            fout << d.data.get()[i];
        }
        return d;
    }
};

} // namespace pipeline
} // namespace om
