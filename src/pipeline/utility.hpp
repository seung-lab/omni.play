#pragma once

#include "pipeline/stage.hpp"
#include <fstream>

namespace om {
namespace pipeline {

template<typename T>
class passthrough : public stage<T, T>
{
protected:
    virtual void inspect(const T* data) {};

public:
    passthrough(out_stage<T>* pred)
        : stage<T, T>(pred)
    {}

    T* operator()(T* data)
    {
        inspect(data);
        return data;
    }

    void cleanup() {
        this->predecessor_->cleanup();
    }

    int out_size() const {
        return this->predecessor_->out_size();
    }
};

template<typename T>
class write_out : public passthrough<T>
{
private:
    std::string fnp_;

public:
    write_out(out_stage<T>* pred, std::string fnp)
        : passthrough<T>(pred)
        , fnp_(fnp)
    { }

protected:
    void inspect(const T* data)
    {
        std::cout << "Writing" << std::endl;
        std::ofstream fout(fnp_.c_str());

        for(int i = 0; i < this->out_size(); i++) {
            fout << data[i];
        }
        std::cout << "Done Writing" << std::endl;
    }
};


} // namespace pipeline
} // namespace om
