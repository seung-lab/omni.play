#ifndef OM_EXTRACT_MESH_H
#define OM_EXTRACT_MESH_H

#include "common/omCommon.h"

#include <QFile>

class OmExtractMesh {
public:
    std::string WriteOutMesh(const OmID segmentationID, const OmSegID segID,
                             const int mipLevel, const int x, const int y, const int z);

private:
    template <typename T>
    void writeFile(const QString& fnp, T const*const data, const uint64_t numBytes)
    {
        QFile file(fnp);

        if(!file.open(QFile::WriteOnly | QFile::Truncate)){
            throw OmIoException("could not open", fnp);
        }

        const uint64_t numBytesWritten = file.write(reinterpret_cast<const char*>(data), numBytes);

        if(numBytesWritten != numBytes){
            throw OmIoException("could not write file", fnp);
        }
    }
};

#endif
