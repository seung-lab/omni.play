#pragma once

/*
 * Segment Object
 *
 * Stored in object pool; values are initialized to 0, except for edgeNumber_
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 * Michael Purcaro - purcaro@gmail.com
 */

#include "segment/omSegmentTypes.h"
#include "segment/omSegmentChildrenTypes.h"
#include "segment/omSegmentEdge.h"
#include "zi/omMutex.h"

class segments;

class OmSegment {
public:
    OmSegment()
        : data_(NULL)
        , segments_(NULL)
        , parent_(NULL)
        , threshold_(0)
        , edgeNumber_(-1)
        , freshnessForMeshes_(0)
    {}

    inline segId value() const {
        return data_->value;
    }

    // color
    void RandomizeColor();

    void reRandomizeColor();

    inline OmColor GetColorInt() const {
        return data_->color;
    }

    inline Vector3f GetColorFloat() const
    {
        return Vector3f( data_->color.red   / 255.,
                         data_->color.green / 255.,
                         data_->color.blue  / 255. );
    }

    void SetColor(const OmColor&);
    void SetColor(const Vector3i&);
    void SetColor(const Vector3f&);

    QString GetNote();
    void SetNote(const QString&);
    QString GetName();
    void SetName(const QString&);
    bool IsSelected();
    void SetSelected(const bool, const bool);
    bool IsEnabled();
    void SetEnabled( const bool);

    uint64_t size() const {
        return data_->size;
    }

    inline void addToSize(const uint64_t inc)
    {
        zi::spinlock::pool<segment_size_mutex_pool_tag>::guard g(data_->value);
        data_->size += inc;
    }

    inline bool IsValidListType() const {
        return om::VALID == static_cast<om::SegListType>(*listType_);
    }

    inline om::SegListType GetListType() const {
        return static_cast<om::SegListType>(*listType_);
    }

    inline void SetListType(const om::SegListType type){
        (*listType_) = static_cast<uint8_t>(type);
    }

    inline OmSegment* getParent() const {
        return parent_;
    }

    inline void setParent(OmSegment* seg){
        parent_ = seg;
    }

    void setParent(OmSegment* segment, const double);

    const segChildCont_t& GetChildren();

    segId RootID();

    OmID GetSegmentationID();

    inline double getThreshold() const {
        return threshold_;
    }

    inline void setThreshold(const double thres){
        threshold_ = thres;
    }

    const om::dataBbox BoundingBox() const;

    inline void ClearBoundingBox()
    {
        zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
        data_->bounds.set(Vector3i::ZERO, Vector3i::ZERO);
    }

    inline void AddToBoundingBox(const om::dataBbox& box)
    {
        zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
        data_->bounds.merge(box);
    }

    inline uint64_t getFreshnessForMeshes() const {
        return freshnessForMeshes_;
    }

    inline void touchFreshnessForMeshes(){
        ++freshnessForMeshes_;
    }

    inline int getEdgeNumber() const {
        return edgeNumber_;
    }

    inline void setEdgeNumber(const int num){
        edgeNumber_ = num;
    }

    inline const OmSegmentEdge& getCustomMergeEdge() const {
        return customMergeEdge_;
    }

    inline void setCustomMergeEdge(const OmSegmentEdge& e){
        customMergeEdge_ = e;
    }

    inline segments* Segments(){
        return segments_;
    }

private:
    OmSegmentDataV4* data_;
    uint8_t* listType_;

    segments* segments_;
    OmSegment* parent_;
    double threshold_;

    int edgeNumber_; // index of edge in main edge list
    OmSegmentEdge customMergeEdge_;
    uint64_t freshnessForMeshes_;

    struct segment_bounds_mutex_pool_tag;
    struct segment_size_mutex_pool_tag;

    friend class OmCacheSegStore;
    friend class dataArchiveSegment;
    friend class OmFindCommonEdge;
    friend class OmPagingPtrStore;
    friend class OmSegmentPage;
};

