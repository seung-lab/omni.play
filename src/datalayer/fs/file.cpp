#include "datalayer/fs/file.h"

#include <QDir>
#include <boost/filesystem.hpp>

int64_t om::file::numBytes(const std::string& fnp)
{
    if(!om::file::exists(fnp)){
        throw common::ioException("file not found", fnp);
    }
    return QFile(std::string::fromStdString(fnp)).size();
}

void om::file::openFileRO(QFile& file)
{
    if(!file.open(QIODevice::ReadOnly)){
        throw common::ioException("could not open file read only", file.fileName());
    }
}

void om::file::openFileRW(QFile& file)
{
    if(!file.open(QIODevice::ReadWrite)){
        throw common::ioException("could not open file read/write", file.fileName());
    }
}

void om::file::openFileWO(QFile& file)
{
    if(!file.open(QIODevice::WriteOnly | QFile::Truncate)){
        throw common::ioException("could not open file for write", file.fileName());
    }
}

void om::file::openFileAppend(QFile& file)
{
    if(!file.open(QIODevice::Append)){
        throw common::ioException("could not open file for write", file.fileName());
    }
}

void om::file::resizeFileNumBytes(QFile* file, const int64_t numBytes)
{
    if(!file->resize(numBytes))
    {
        throw common::ioException("could not resize file to "
                            + om::string::num(numBytes)
                            + " bytes");
    }
}

void om::file::rmFile(const std::string& fnp)
{
    const std::string f = std::string::fromStdString(fnp);

    if(QFile::exists(f)){
        if(!QFile::remove(f)){
            throw common::ioException("could not remove previous file", f);
        }
    }
}

bool om::file::exists(const std::string& fnp) {
    return QFile::exists(std::string::fromStdString(fnp));
}

std::string om::file::tempPath(){
    return QDir::tempPath().toStdString();
}

void om::file::mvFile(const std::string& old_fnp, const std::string& new_fnp)
{
    try {
        boost::filesystem::rename(old_fnp, new_fnp);

    } catch(...){
        throw common::ioException("could not mv file", old_fnp);
    }
}

void om::file::cpFile(const std::string& from_fnp, const std::string& to_fnp)
{
    try {
        rmFile(to_fnp);
        boost::filesystem::copy_file(from_fnp, to_fnp);

    } catch(...){
        throw common::ioException("could not mv file", from_fnp);
    }
}
