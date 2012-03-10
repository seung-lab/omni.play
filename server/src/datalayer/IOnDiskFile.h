#pragma once

namespace om {
namespace datalayer {

template <typename T>
class IOnDiskFile {
public:
    virtual ~IOnDiskFile(){}

    virtual uint64_t Size() const = 0;
    virtual void Flush(){}
    virtual T* GetPtr() const = 0;
    virtual T* GetPtrWithOffset(const int64_t offset) const = 0;
    virtual std::string GetBaseFileName() const = 0;
};

}
}
