#pragma once

#include "common/exception.h"
#include <string>
#include "zi/concurrency.hpp"
#include <fstream>

namespace om {
namespace utility {    

class fileLogger {
private:
    std::string outFile_;

    std::ofstream out_;
    
    zi::spinlock lock_;

public:
    fileLogger()
    {}

    ~fileLogger()
    {
        out_.close();
    }

    void Start(const std::string& name)
    {
        outFile_ = name;

        if(out_.open(name)) {
            std::cout << "writing log file " << outFile_ << std::endl;

        } else{
            throw common::ioException("could not open file", outFile_);
        }
    }

    void Log(const std::string& str)
    {
        zi::guard g(lock_);
        (*out_) << str << "\n";
    }

    void Log(const std::ostringstream& stm) {
        Log(stm.str());
    }
};

} // namespace utility
} // namespace om