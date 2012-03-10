#pragma once

#include "common/exception.h"
#include "utility/smartPtr.hpp"

#include <vector>
#include <fstream>


#include "boost/filesystem.hpp"

namespace om {
namespace file {

int64_t numBytes(const std::string& fnp);

void resizeFileNumBytes(const std::string* fnp, const int64_t newSize);
inline void resizeFileNumBytes(const std::string& fnp, const int64_t newSize){
    resizeFileNumBytes(&fnp, newSize);
}

template <typename T>
void resizeFileNumElements(const std::string& fnp, const int64_t numElements){
    resizeFileNumBytes(&fnp, sizeof(T) * numElements);
}

template <typename T>
void resizeFileNumElements(const std::string* fnp, const int64_t numElements){
    resizeFileNumBytes(fnp, sizeof(T) * numElements);
}

void rmFile(const std::string& fnp);
void mvFile(const std::string& old_fnp, const std::string& new_fnp);
void cpFile(const std::string& old_fnp, const std::string& new_fnp);

bool exists(const std::string& fnp);
std::string absolute(const std::string& fnp);

std::string tempPath();

template <typename T>
boost::shared_ptr<T> readAll(std::string* fnp)
{
    const int64_t bytes = numBytes(*fnp);

    if(0 != bytes % sizeof(T)){
        throw ioException("file size not even multiple of sizeof(type)");
    }

    boost::shared_ptr<T> ret =
        utility::smartPtr<T>::MallocNumBytes(bytes, common::DONT_ZERO_FILL);

    char* dataChar = reinterpret_cast<char*>(ret.get());

    std::fstream file(*fnp, std::fstream::in | std::fstream::binary);
    const int64_t bytesRead = file.read(dataChar, bytes);

    if(bytesRead != bytes){
        throw ioException("could not read entire file");
    }

    return ret;
}


template <typename T>
boost::shared_ptr<T> readAll(std::string& fnp) {
    return readAll<T>(&fnp);
}

template <typename T>
void writeVec(std::string& fnp, const std::vector<T>& vec)
{
    resizeFileNumElements<T>(fnp, vec.size());

    const char* data = reinterpret_cast<const char*>(&vec[0]);

    const int64_t bytes = vec.size() * sizeof(T);

    std::fstream file(fnp, std::fstream::out | std::fstream::binary);

    if(!file.good()){
        throw ioException("could not fully write file", fnp);
    }
}

template <typename T>
void createFileNumElements(const std::string& fnp, const int64_t numElements)
{
    const int64_t numBytes = numElements * sizeof(T);

    resizeFileNumBytes(fnp, numBytes);
}

template <typename T>
void writeNumElements(const std::string& fnp, const boost::shared_ptr<T> ptr,
                      const int64_t numElements)
{
    const int64_t numBytes = numElements * sizeof(T);

    resizeFileNumBytes(fnp, numBytes);

    const char* data = reinterpret_cast<const char*>(ptr.get());

    std::fstream file(fnp.c_str(), std::fstream::out | std::fstream::binary);

    if(!file.good()){
        throw ioException("could not fully write file", fnp);
    }
}

template <typename T>
void writeStrings(std::string& file, const T& strings)
{
    std::fstream out(file, std::fstream::out);

    FOR_EACH(iter, strings)
    {
        out << *iter << "\n";
    }
}

template <typename T, typename PROGRESS>
void writeStrings(std::string& file, const T& strings, PROGRESS* progress)
{
    progress->SetTotal(strings.size());

    std::fstream out(&file);

    FOR_EACH(iter, strings)
    {
        out << *iter << "\n";
        progress->SetDone(1);
    }
}

} // namespace file
} // namespace om

