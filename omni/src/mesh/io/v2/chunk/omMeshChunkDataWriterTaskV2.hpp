#ifndef OM_MESH_CHUNK_DATA_WRITER_TASK_V2_HPP
#define OM_MESH_CHUNK_DATA_WRITER_TASK_V2_HPP

#include "mesh/omMeshCoord.h"

class OmMeshChunkDataWriterTaskV2{
private:
    const OmMeshCoord meshCoord_;
    const QString fnp_;
    const int64_t offsetIntoFile_;
    const int64_t numBytes_;

public:
    OmMeshChunkDataWriterTaskV2(const OmMeshCoord& meshCoord,
                                const QString fnp,
                                const uint64_t offsetIntoFile,
                                const uint64_t numBytes)
        : meshCoord_(meshCoord)
        , fnp_(fnp)
        , offsetIntoFile_(offsetIntoFile)
        , numBytes_(numBytes)
    {}

    template <typename T>
    void Write(const std::vector<T>& vec)
    {
        const char* dataCharPtr =
            reinterpret_cast<const char*>(&vec[0]);
        doWrite(dataCharPtr);
    }

    template <typename T>
    void Write(om::shared_ptr<T> dataRawPtr)
    {
        const char* dataCharPtr =
            reinterpret_cast<const char*>(dataRawPtr.get());
        doWrite(dataCharPtr);
    }

private:

    void doWrite(const char* dataCharPtr)
    {
        QFile writer(fnp_);
        if( !writer.open(QIODevice::ReadWrite)) {
            throw OmIoException("could not open", fnp_);
        }

        if(!writer.seek(offsetIntoFile_)){
            throw OmIoException("could not seek to " +
                                om::string::num(offsetIntoFile_));
        }

        const int64_t bytesWritten = writer.write(dataCharPtr, numBytes_);

        if(bytesWritten != numBytes_){
            std::cout << "could not write data; numBytes is " << numBytes_
                      << ", but only wrote " << bytesWritten << "\n"
                      << std::flush;
            throw OmIoException("could not write fully file", fnp_);
        }
    }
};

#endif
