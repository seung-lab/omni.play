#pragma once

#include "common/common.h"
#include "mesh/io/chunk/meshChunkTypes.h"
#include "utility/smartPtr.hpp"

#include <fstream>

namespace om {
namespace mesh {

class chunkDataReader{
private:
    const std::string fnp_;

public:
    chunkDataReader(const std::string& fnp)
        : fnp_(fnp)
    {}

    ~chunkDataReader()
    {}

    // no locking needed
    template <typename T>
    boost::shared_ptr<T> Read(const filePart& entry)
    {
        const int64_t numBytes = entry.totalBytes;

        assert(numBytes);

        boost::shared_ptr<T> ret = utility::smartPtr<T>::MallocNumBytes(numBytes);

        char* dataCharPtr = reinterpret_cast<char*>(ret.get());

        std::ifstream reader(fnp_.c_str(), std::ios::in | std::ios::binary);

        if(!reader.seekg(entry.offsetIntoFile))
        {
            std::stringstream ss;
            ss << "could not seek to " << entry.offsetIntoFile;
            throw common::ioException(ss.str());
        }

        reader.read(dataCharPtr, numBytes);

        if(reader.fail())
        {
            std::cout << "could not read data; numBytes is " << numBytes << "\n"
                      << std::flush;
            throw common::ioException("could not read fully file", fnp_);
        }

        return ret;
    }
};

} // namespace mesh
} // namespace om
