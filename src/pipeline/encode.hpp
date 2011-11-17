#pragma once

#include "pipeline/stage.hpp"
#include "b64/encode.h"

#include "boost/scoped_ptr.hpp"

namespace om {
namespace pipeline {

class encode : public stage
{
public:
    template<typename T>
    data_var operator()(const data<T>& in) const {
        return doEncode(reinterpret_cast<const char*>(in.data.get()), in.size);
    }

    data_var operator()(const data<char>& in) const {
        return doEncode(in.data.get(), in.size);
    }

private:
    data_var doEncode(const char* in, int64_t size) const
    {
        base64::encoder e;

        std::cout << "Encoding" << std::endl;

        data<char> out;
        out.size = 2 * size;
        out.data.reset(new char[out.size]);

        char* dest = out.data.get();

        int written = e.encode(in, size, dest);
        written += e.encode_end(&dest[written]);

        dest[written] = '\0';
        out.size = written + 1;

        return out;
    }
};

}
}
