#include "datalayer/fs/file.h"

#include <boost/filesystem.hpp>

namespace om {
namespace file {

int64_t numBytes(const std::string& fnp)
{
    if(!exists(fnp)){
        throw common::ioException("file not found", fnp);
    }
    return boost::filesystem::file_size(fnp);
}

void resizeFileNumBytes(const std::string* file, const int64_t numBytes)
{
    boost::system::error_code ec;
    boost::filesystem::resize_file(*file, numBytes, ec);
    if(ec)
    {
        throw common::ioException(str(boost::format("could not resize file to %1% bytes")
                                      % numBytes));
    }
}

void rmFile(const std::string& fnp)
{
    if(exists(fnp)){
        boost::system::error_code ec;
        boost::filesystem::remove(fnp, ec);
        if (ec){
            throw common::ioException("could not remove previous file", fnp);
        }
    }
}

bool exists(const std::string& fnp) {
    return boost::filesystem::exists(fnp);
}

std::string tempPath(){
    return boost::filesystem::temp_directory_path().string();
}

void mvFile(const std::string& old_fnp, const std::string& new_fnp)
{
    try {
        boost::filesystem::rename(old_fnp, new_fnp);

    } catch(...){
        throw common::ioException("could not mv file", old_fnp);
    }
}

void cpFile(const std::string& from_fnp, const std::string& to_fnp)
{
    try {
        rmFile(to_fnp);
        boost::filesystem::copy_file(from_fnp, to_fnp);

    } catch(...){
        throw common::ioException("could not mv file", from_fnp);
    }
}

} // namespace file
} // namespace om
