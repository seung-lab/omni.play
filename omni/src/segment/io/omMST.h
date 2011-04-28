#ifndef OM_MST_H
#define OM_MST_H

#include "segment/io/omMSTtypes.h"
#include "utility/omSharedPtr.hpp"
#include "zi/omMutex.h"

#include <QString>

class OmSegmentation;
template <class> class OmIOnDiskFile;
template <class> class OmMemMappedFileReadQT;
template <class> class OmMemMappedFileWriteQT;
template <class> class OmFileReadQT;
template <class> class OmFileWriteQT;
class QDataStream;

class OmMST {
private:
    static const double defaultThreshold_ = 0.9;

public:
    OmMST(OmSegmentation* segmentation);

    ~OmMST()
    {}

    static double DefaultThreshold(){
        return defaultThreshold_;
    }

    void Read();
    void Flush();

    void Import(const std::vector<OmMSTImportEdge>& edges);

    inline bool IsValid() const {
        return numEdges_ > 0;
    }

    inline uint32_t NumEdges() const {
        return numEdges_;
    }

    inline double UserThreshold() const {
        return userThreshold_;
    }

    void SetUserThreshold(const double t);

    inline OmMSTEdge* Edges(){
        return edges_;
    }

private:
    OmSegmentation *const segmentation_;
    const QString fnpActual_;
    // const QString fnpWorking_;

    uint32_t numEdges_;
    double userThreshold_;

    zi::rwmutex thresholdLock_;

    om::shared_ptr<OmIOnDiskFile<OmMSTEdge> > edgesPtr_;
    OmMSTEdge* edges_;

    void create();
    void convert();
    // void writeToActualFile();

    // read MST via full load into memory
    typedef OmFileReadQT<OmMSTEdge> reader_t;
    typedef OmFileWriteQT<OmMSTEdge> writer_t;

    // read MST via mem map
    // typedef OmMemMappedFileReadQT<OmMSTEdge> reader_t;
    // typedef OmMemMappedFileWriteQT<OmMSTEdge> writer_t;

    QString filePathActual();
    // QString filePathWorking();

    friend class SegmentTests1;

    friend class OmDataArchiveProjectImpl;
    friend QDataStream &operator<<(QDataStream& out, const OmSegmentation& seg);
};

#endif
