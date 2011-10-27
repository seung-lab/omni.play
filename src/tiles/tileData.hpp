#pragma once

#include "boost/shared_ptr.hpp"

namespace om {
namespace tiles {

class tileDataInterface {
public:
    virtual void* data() = 0;
    virtual const void* data() const = 0;
};

template<typename T>
class tileData : public tileDataInterface
{
private:
    boost::shared_ptr<T> data_;

public:
    tileData(boost::shared_ptr<T> data)
        : data_(data)
    {}

    void* data() {
        return data_.get();
    }

    const void* data() const {
        return data_.get();
    }
};

}
}
