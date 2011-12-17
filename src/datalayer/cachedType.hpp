#pragma once

namespace om {
namespace datalayer {

template <typename T>
class cachedType
{
protected:
    T* data_;

public:
    cachedType()
        : data_(NULL)
    {}

    cachedType(char* data, size_t size)
    {
        if(size != sizeof(T)) {
            throw argException("Data wrong size.");
        }

        data_ = reinterpret_cast<T*>(data);
    }
};

}} // namespace om::datalayer::
