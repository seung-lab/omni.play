#pragma once

#include "project/details/segmentationManager.h"
#include "project/project.h"
#include "project/details/projectVolumes.h"
#include "segment/segment.h"
#include "segment/segments.h"
#include "volume/segmentation.h"

namespace om {
namespace volume {

class SegmentationDataWrapper {
public:
    static const idset& ValidIDs(){
        return project::Volumes().Segmentations().GetValidSegmentationIds();
    }

    static inline const std::vector<segmentation*> GetPtrVec(){
        return project::Volumes().Segmentations().GetPtrVec();
    }

    static void Remove(const common::id id){
        project::Volumes().Segmentations().RemoveSegmentation(id);
    }

private:
    common::id id_;
    mutable boost::optional<segmentation&> segmentation_;

public:
    SegmentationDataWrapper()
        : id_(0)
    {}

    explicit SegmentationDataWrapper(const common::id ID)
        : id_(ID)
    {}

    SegmentationDataWrapper(const SegmentationDataWrapper& sdw)
        : id_(sdw.id_)
    {}

    explicit SegmentationDataWrapper(segment* seg)
        : id_(seg->GetSegmentationID())
    {}

    inline void set(const SegmentationDataWrapper& sdw){
        id_ = sdw.id_;
    }

    SegmentationDataWrapper& operator =(const SegmentationDataWrapper& sdw)
    {
        if (this != &sdw){
            id_ = sdw.id_;
            segmentation_ = sdw.segmentation_;
        }
        return *this;
    }

    bool operator ==(const SegmentationDataWrapper& sdw) const {
        return id_ == sdw.id_;
    }

    bool operator !=(const SegmentationDataWrapper& sdw) const {
        return !(*this == sdw);
    }

    inline common::id GetSegmentationID() const {
        return id_;
    }

    inline common::id GetID() const {
        return id_;
    }

    segmentation& Create()
    {
        segmentation& s = project::Volumes().Segmentations().AddSegmentation();
        id_ = s.GetID();
        printf("create segmentation %d\n", id_);
        segmentation_ =	boost::optional<segmentation&>(s);
        return s;
    }

    void Remove()
    {
        Remove(id_);
        id_ = 0;
        segmentation_.reset();
    }

    inline ObjectType getType() const {
        return SEGMENTATION;
    }

    inline bool IsValidWrapper() const {
        return IsSegmentationValid();
    }

    inline bool IsSegmentationValid() const
    {
        if(!id_){
            return false;
        }

        return project::Volumes().Segmentations().IsSegmentationValid(id_);
    }

    inline segmentation& GetSegmentation() const
    {
        if(!segmentation_){
            //printf("cached segmentation...\n");
            segmentation_ =
                boost::optional<segmentation&>(project::Volumes().Segmentations().GetSegmentation(id_));
        }
        return *segmentation_;
    }

    inline segmentation* GetSegmentationPtr() const
    {
        segmentation& seg = GetSegmentation();
        return &seg;
    }

    inline QString GetName() const {
        return QString::fromStdString(GetSegmentation().GetName());
    }

    inline bool isEnabled() const {
        return project::Volumes().Segmentations().IsSegmentationEnabled(id_);
    }

    inline QString getNote() const {
        return GetSegmentation().GetNote();
    }

    inline uint32_t getNumberOfSegments() const {
        return Segments()->GetNumSegments();
    }

    inline uint32_t getNumberOfTopSegments() const {
        return Segments()->GetNumTopSegments();
    }

    inline OmSegments* Segments() const {
        return GetSegmentation().Segments();
    }

    inline OmMST* MST() const {
        return GetSegmentation().MST();
    }

    inline uint32_t getMaxSegmentValue() const {
        return Segments()->getMaxValue();
    }

    inline OmSegmentLists* SegmentLists() const {
        return GetSegmentation().SegmentLists();
    }

    inline OmValidGroupNum* ValidGroupNum() const {
        return GetSegmentation().ValidGroupNum();
    }

    inline const Vector3f GetDataResolution() const {
        return GetSegmentation().Coords().GetResolution();
    }

    inline const OmSegIDsSet GetSelectedSegmentIDs() const {
        return Segments()->GetSelectedSegmentIDs();
    }

    inline OmGroups* Groups() const {
        return GetSegmentation().Groups();
    }

    inline bool IsBuilt() const {
        return GetSegmentation().IsBuilt();
    }
};

}
}
