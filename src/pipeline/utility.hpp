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
        std::cout << "Writing" << std::endl;
        std::ofstream fout(fnp_.c_str());

        for(int i = 0; i < d.size; i++) {
            fout << d.data.get()[i];
        }
        std::cout << "Done Writing" << std::endl;
        return d;
    }
};

} // namespace pipeline
} // namespace om
