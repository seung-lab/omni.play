#ifndef OM_MST_H
#define OM_MST_H

#include "zi/omMutex.h"
#include "segment/io/omMSTtypes.h"

#include <boost/shared_ptr.hpp>

class OmSegmentation;
template <class> class OmIOnDiskFile;
template <class> class OmFileReadQT;
template <class> class OmFileWriteQT;
class QDataStream;

class OmMST {
private:
    static const double defaultThresholdSize_ = 0.9;

public:
    OmMST(OmSegmentation* segmentation);

    ~OmMST()
    {}

    static double DefaultThresholdSize(){
        return defaultThresholdSize_;
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
    uint32_t numEdges_;
    double userThreshold_;

    zi::rwmutex thresholdLock_;

    boost::shared_ptr<OmIOnDiskFile<OmMSTEdge> > edgesPtr_;
    OmMSTEdge* edges_;

    void create();
    void convert();

    typedef OmFileReadQT<OmMSTEdge> reader_t;
    typedef OmFileWriteQT<OmMSTEdge> writer_t;

    std::string memMapPath();

    friend class SegmentTests1;

    friend class OmDataArchiveProject;
    friend QDataStream &operator<<(QDataStream& out, const OmSegmentation& seg);
};

#endif
