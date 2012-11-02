#pragma once

#include "project/details/omSegmentationManager.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegments.h"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"

class SegmentDataWrapper {
private:
    OmSegID segmentID_;
    OmID segmentationID_;

    // not allowed--allows wrappers to be implicitly converted!!!
    explicit SegmentDataWrapper(const SegmentationDataWrapper& sdw);

public:
    SegmentDataWrapper()
        : segmentID_(0)
        , segmentationID_(0)
    {}

    SegmentDataWrapper(const OmID segmentationID,
                       const OmSegID segmentID)
        : segmentID_(segmentID)
        , segmentationID_(segmentationID)
    {}

    SegmentDataWrapper(const OmSegmentation* segmentation,
                       const OmSegID segmentID)
        : segmentID_(segmentID)
        , segmentationID_(segmentation->getID())
    {}

    explicit SegmentDataWrapper(OmSegment* seg)
        : segmentID_(seg->value())
        , segmentationID_(seg->GetSegmentationID())
    {}

    SegmentDataWrapper(const SegmentationDataWrapper& sdw, const OmSegID segID)
        : segmentID_(segID)
        , segmentationID_(sdw.GetSegmentationID())
    {}

    inline void set(const SegmentDataWrapper& sdw)
    {
        segmentID_ = sdw.segmentID_;
        segmentationID_ = sdw.segmentationID_;
    }

    inline void SetSegmentID(const OmSegID segID){
        segmentID_ = segID;
    }

    SegmentDataWrapper& operator=(const SegmentDataWrapper& sdw)
    {
        if (this != &sdw)
        {
            segmentID_ = sdw.segmentID_;
            segmentationID_ = sdw.segmentationID_;
        }

        return *this;
    }

    bool operator<(const SegmentDataWrapper& rhs) const
    {
        if(segmentationID_ != rhs.segmentationID_){
            return segmentationID_ < rhs.segmentationID_;
        }

        return segmentID_ < rhs.segmentID_;
    }

    inline OmSegID GetSegmentID() const {
        return segmentID_;
    }

    bool operator ==(const SegmentDataWrapper& sdw) const {
        return segmentID_ == sdw.segmentID_ && segmentationID_ == sdw.segmentationID_;
    }

    bool operator !=(const SegmentDataWrapper& sdw) const {
        return !(*this == sdw);
    }

    SegmentationDataWrapper MakeSegmentationDataWrapper() const {
        return SegmentationDataWrapper(segmentationID_);
    }

    inline bool IsSegmentationValid() const
    {
        if(!segmentationID_){
            return false;
        }

        return OmProject::Volumes().Segmentations().IsSegmentationValid(segmentationID_);
    }

    inline bool IsSegmentValid() const
    {
        if(!segmentID_ && !segmentationID_){
            return false;
        }

        return IsSegmentationValid() && Segments()->IsSegmentValid(segmentID_);
    }

    inline bool IsValidWrapper() const {
        return IsSegmentValid();
    }

    inline QString getName() const {
        return GetSegment()->GetName();
    }

    inline QString GetSegmentationName() const {
        return QString::fromStdString(GetSegmentation().GetName());
    }

    inline bool isSelected() const {
        return Segments()->IsSegmentSelected(segmentID_);
    }

    inline void setSelected(const bool isSelected,
                            const bool addToRecentList) const {
        Segments()->setSegmentSelected(segmentID_, isSelected, addToRecentList);
    }

    inline bool isEnabled() const {
        return Segments()->isSegmentEnabled(segmentID_);
    }

    inline void setEnabled(const bool enabled) const {
        Segments()->setSegmentEnabled(segmentID_, enabled );
    }

    inline QString getNote() const {
        return GetSegment()->GetNote();
    }

    inline void setNote(const QString& str) const {
        GetSegment()->SetNote(str);
    }

    inline QString getIDstr() const {
        return QString("%1").arg(getID());
    }

    inline om::common::Color GetColorInt() const {
        return GetSegment()->GetColorInt();
    }

    inline Vector3f GetColorFloat() const {
        return GetSegment()->GetColorFloat();
    }

    inline void SetColor(const om::common::Color& color) const {
        GetSegment()->SetColor(color);
    }

    inline void SetColor(const Vector3f& color) const {
        GetSegment()->SetColor(color);
    }

    inline void setName( const QString& str ) const {
        GetSegment()->SetName( str );
    }

    inline OmSegmentation& GetSegmentation() const {
        return OmProject::Volumes().Segmentations().GetSegmentation(segmentationID_);
    }

    inline OmSegmentation* GetSegmentationPtr() const
    {
        OmSegmentation& segmentation = GetSegmentation();
        return &segmentation;
    }

    inline OmSegment* GetSegment() const {
        return Segments()->GetSegment( segmentID_ );
    }

    inline OmSegments* Segments() const {
        return GetSegmentation().Segments();
    }

    inline uint64_t getSize() const {
        return GetSegment()->size();
    }

    inline OmID GetSegmentationID() const {
        return segmentationID_;
    }

    inline OmSegID getID() const {
        return segmentID_;
    }

    inline OmSegID GetVoxelValue(const om::coords::Global& dataClickPoint) const {
        return GetSegmentation().GetVoxelValue(dataClickPoint);
    }

    inline OmSegID FindRootID() const {
        return Segments()->findRootID(segmentID_);
    }

    inline OmSegment* FindRoot() const {
        return Segments()->findRoot(segmentID_);
    }

    inline void RandomizeColor() const {
        GetSegment()->reRandomizeColor();
    }

    inline int64_t GetSizeWithChildren() const {
        return GetSegmentation().SegmentLists()->GetSizeWithChildren(segmentID_);
    }

    inline SegmentDataWrapper FindRootSDW() const {
        return SegmentDataWrapper(segmentationID_, FindRootID());
    }

    friend std::ostream& operator<<(std::ostream &out,
                                    const SegmentDataWrapper& s)
    {
        out << "(segmentation " << s.segmentationID_
            << ", segment " << s.segmentID_ << ")";
        return out;
    }
};

