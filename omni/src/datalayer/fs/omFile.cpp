#include "datalayer/fs/omFile.hpp"

#include <QDir>
#include <boost/filesystem.hpp>

int64_t om::file::numBytes(const std::string& fnp)
{
    if(!om::file::exists(fnp)){
        throw OmIoException("file not found", fnp);
    }
    return QFile(QString::fromStdString(fnp)).size();
}

void om::file::openFileRO(QFile& file)
{
    if(!file.open(QIODevice::ReadOnly)){
        throw OmIoException("could not open file read only", file.fileName());
    }
}

void om::file::openFileRW(QFile& file)
{
    if(!file.open(QIODevice::ReadWrite)){
        throw OmIoException("could not open file read/write", file.fileName());
    }
}

void om::file::openFileWO(QFile& file)
{
    if(!file.open(QIODevice::WriteOnly)){
        throw OmIoException("could not open file for write", file.fileName());
    }
}

void om::file::resizeFileNumBytes(QFile* file, const int64_t numBytes)
{
    if(!file->resize(numBytes))
    {
        throw OmIoException("could not resize file to "
                            + om::string::num(numBytes)
                            + " bytes");
    }
}

void om::file::rmFile(const std::string& fnp)
{
    const QString f = QString::fromStdString(fnp);

    if(QFile::exists(f)){
        if(!QFile::remove(f)){
            throw OmIoException("could not remove previous file", f);
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
        throw OmIoException("could not mv file", old_fnp);
    }
}
