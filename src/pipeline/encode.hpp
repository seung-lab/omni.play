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

        int written = e.encode(in, size, out.data.get());
        written += e.encode_end(&out.data[written]);

        out.data[written] = '\0';
        out.size = written + 1;

        return out;
    }
};

data_var operator>>(const data_var& d, const encode& v) {
    return boost::apply_visitor(v, d);
}

}
}
