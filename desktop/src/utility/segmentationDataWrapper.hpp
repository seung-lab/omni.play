#pragma once

#include "project/details/omSegmentationManager.h"
#include "project/omProject.h"
#include "project//details/omProjectVolumes.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "volume/omSegmentation.h"

class SegmentationDataWrapper {
public:
    static const om::common::IDSet& ValidIDs(){
        return OmProject::Volumes().Segmentations().GetValidSegmentationIds();
    }

    static inline const std::vector<OmSegmentation*> GetPtrVec(){
        return OmProject::Volumes().Segmentations().GetPtrVec();
    }

    static void Remove(const om::common::ID id){
        OmProject::Volumes().Segmentations().RemoveSegmentation(id);
    }

private:
    om::common::ID id_;
    mutable boost::optional<OmSegmentation&> segmentation_;

public:
    SegmentationDataWrapper()
        : id_(0)
    {}

    explicit SegmentationDataWrapper(const om::common::ID ID)
        : id_(ID)
    {}

    SegmentationDataWrapper(const SegmentationDataWrapper& sdw)
        : id_(sdw.id_)
    {}

    explicit SegmentationDataWrapper(OmSegment* seg)
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

    inline om::common::ID GetSegmentationID() const {
        return id_;
    }

    inline om::common::ID GetID() const {
        return id_;
    }

    OmSegmentation& Create()
    {
        OmSegmentation& s = OmProject::Volumes().Segmentations().AddSegmentation();
        id_ = s.GetID();
        printf("create segmentation %d\n", id_);
        segmentation_ =	boost::optional<OmSegmentation&>(s);
        return s;
    }

    void Remove()
    {
        Remove(id_);
        id_ = 0;
        segmentation_.reset();
    }

    inline om::common::ObjectType getType() const {
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

        return OmProject::Volumes().Segmentations().IsSegmentationValid(id_);
    }

    inline OmSegmentation& GetSegmentation() const
    {
        if(!segmentation_){
            //printf("cached segmentation...\n");
            segmentation_ =
                boost::optional<OmSegmentation&>(OmProject::Volumes().Segmentations().GetSegmentation(id_));
        }
        return *segmentation_;
    }

    inline OmSegmentation* GetSegmentationPtr() const
    {
        OmSegmentation& seg = GetSegmentation();
        return &seg;
    }

    inline QString GetName() const {
        return QString::fromStdString(GetSegmentation().GetName());
    }

    inline bool isEnabled() const {
        return OmProject::Volumes().Segmentations().IsSegmentationEnabled(id_);
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

    inline const om::common::SegIDSet GetSelectedSegmentIDs() const {
        return Segments()->GetSelectedSegmentIDs();
    }

    inline OmGroups* Groups() const {
        return GetSegmentation().Groups();
    }

    inline bool IsBuilt() const {
        return GetSegmentation().IsBuilt();
    }
};
