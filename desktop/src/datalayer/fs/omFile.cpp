#include "datalayer/fs/omFile.hpp"

#include <QDir>
#include <boost/filesystem.hpp>

int64_t om::file::numBytes(const std::string& fnp)
{
    if(!om::file::exists(fnp)){
        throw IoException("file not found", fnp);
    }
    return QFile(QString::fromStdString(fnp)).size();
}

void om::file::openFileRO(QFile& file)
{
    if(!file.open(QIODevice::ReadOnly)){
        throw IoException("could not open file read only", file.fileName());
    }
}

void om::file::openFileRW(QFile& file)
{
    if(!file.open(QIODevice::ReadWrite)){
        throw IoException("could not open file read/write", file.fileName());
    }
}

void om::file::openFileWO(QFile& file)
{
    if(!file.open(QIODevice::WriteOnly | QFile::Truncate)){
        throw IoException("could not open file for write", file.fileName());
    }
}

void om::file::openFileAppend(QFile& file)
{
    if(!file.open(QIODevice::Append)){
        throw IoException("could not open file for write", file.fileName());
    }
}

void om::file::resizeFileNumBytes(QFile* file, const int64_t numBytes)
{
    if(!file->resize(numBytes))
    {
        throw IoException("could not resize file to "
                            + om::string::num(numBytes)
                            + " bytes");
    }
}

void om::file::rmFile(const std::string& fnp)
{
    const QString f = QString::fromStdString(fnp);

    if(QFile::exists(f)){
        if(!QFile::remove(f)){
            throw IoException("could not remove previous file", f);
        }
    }
}

bool om::file::exists(const std::string& fnp) {
    return QFile::exists(QString::fromStdString(fnp));
}

std::string om::file::tempPath(){
    return QDir::tempPath().toStdString();
}

void om::file::mvFile(const std::string& old_fnp, const std::string& new_fnp)
{
    try {
        boost::filesystem::rename(old_fnp, new_fnp);

    } catch(...){
        throw IoException("could not mv file", old_fnp);
    }
}

void om::file::cpFile(const std::string& from_fnp, const std::string& to_fnp)
{
    try {
        rmFile(to_fnp);
        boost::filesystem::copy_file(from_fnp, to_fnp);

    } catch(...){
        throw IoException("could not mv file", from_fnp);
    }
}
