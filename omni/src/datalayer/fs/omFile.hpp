#ifndef OM_FILE_HPP
#define OM_FILE_HPP

#include "common/omException.h"
#include "utility/omSmartPtr.hpp"

#include <vector>
#include <QFile>

namespace om {
namespace file {

void openFileRO(QFile& file);
void openFileRW(QFile& file);
void openFileWO(QFile& file);

int64_t numBytes(const std::string& fnp);

void resizeFileNumBytes(QFile* file, const int64_t newSize);
inline void resizeFileNumBytes(QFile& file, const int64_t newSize){
    resizeFileNumBytes(&file, newSize);
}

template <class T>
void resizeFileNumElements(QFile& file, const int64_t numElements){
    resizeFileNumBytes(&file, sizeof(T) * numElements);
}

template <class T>
void resizeFileNumElements(QFile* file, const int64_t numElements){
    resizeFileNumBytes(file, sizeof(T) * numElements);
}

void rmFile(const std::string& fnp);
void mvFile(const std::string& old_fnp, const std::string& new_fnp);
void cpFile(const std::string& old_fnp, const std::string& new_fnp);

bool exists(const std::string& fnp);

std::string tempPath();

template <class PTR>
void openFileRO(PTR& file, const std::string& fnp)
{
    file.reset(new QFile(QString::fromStdString(fnp)));
    if(!file->open(QIODevice::ReadOnly)){
        throw OmIoException("could not open file read only", fnp);
    }
}

template <class PTR>
void openFileRW(PTR& file, const std::string& fnp)
{
    file.reset(new QFile(QString::fromStdString(fnp)));
    if(!file->open(QIODevice::ReadWrite)){
        throw OmIoException("could not open file read/write", fnp);
    }
}

template <class PTR>
void openFileWO(PTR& file, const std::string& fnp)
{
    file.reset(new QFile(QString::fromStdString(fnp)));

    if(!file->open(QIODevice::WriteOnly)){
        throw OmIoException("could not open file for writing", fnp);
    }
}

template <class T>
T* mapFile(QFile* file)
{
    uchar* map = file->map(0, file->size());

    if(!map){
        throw OmIoException("could not map file");
    }

    file->close();

    return reinterpret_cast<T*>(map);
}

template <class T, class PTR>
T* mapFile(PTR& file)
{
    uchar* map = file->map(0, file->size());

    if(!map){
        throw OmIoException("could not map file");
    }

    file->close();

    return reinterpret_cast<T*>(map);
}

template <class T>
om::shared_ptr<T> readAll(QFile* file)
{
    const int64_t numBytes = file->size();

    if(0 != numBytes % sizeof(T)){
        throw OmIoException("file size not even multiple of sizeof(type)");
    }

    om::shared_ptr<T> ret =
        OmSmartPtr<T>::MallocNumBytes(numBytes, om::DONT_ZERO_FILL);

    char* dataChar = reinterpret_cast<char*>(ret.get());

    const int64_t numBytesRead = file->read(dataChar, numBytes);

    if(numBytesRead != numBytes){
        throw OmIoException("could not read entire file");
    }

    return ret;
}


template <class T>
om::shared_ptr<T> readAll(QFile& file) {
    return readAll<T>(&file);
}

template <class T>
void writeVec(QFile& file, const std::vector<T>& vec)
{
    resizeFileNumElements<T>(file, vec.size());

    const char* data = reinterpret_cast<const char*>(&vec[0]);

    const int64_t numBytes = vec.size() * sizeof(T);

    const int numBytesWritten = file.write(data, numBytes);

    if(numBytesWritten != numBytes){
        throw OmIoException("could not fully write file", file.fileName());
    }
}

template <class T>
void writeNumElements(QFile& file, const om::shared_ptr<T> ptr,
                      const int64_t numElements)
{
    const int64_t numBytes = numElements * sizeof(T);

    resizeFileNumBytes(file, numBytes);

    const char* data = reinterpret_cast<const char*>(ptr.get());

    const int numBytesWritten = file.write(data, numBytes);

    if(numBytesWritten != numBytes){
        throw OmIoException("could not fully write file", file.fileName());
    }
}

template <class T>
void createFileNumElements(const std::string& fnp, const int64_t numElements)
{
    QFile file(QString::fromStdString(fnp));

    openFileWO(file);

    om::shared_ptr<T> empty = OmSmartPtr<T>::MallocNumElements(numElements,
                                                                  om::ZERO_FILL);

    writeNumElements(file, empty, numElements);
}

template <class T>
void createFileFromData(const std::string& fnp, const om::shared_ptr<T> ptr,
                        const int64_t numElements)
{
    QFile file(QString::fromStdString(fnp));

    openFileWO(file);

    writeNumElements(file, ptr, numElements);
}

} // namespace file
} // namespace om

#endif
