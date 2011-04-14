#ifndef OM_SEGMENT_VALIDATE_ACTION_IMPL_HPP
#define OM_SEGMENT_VALIDATE_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentValidation.hpp"

class OmSegmentValidateActionImpl {
private:
    SegmentationDataWrapper sdw_;
    bool valid_;
    om::shared_ptr<std::set<OmSegment*> > selectedSegments_;

public:
    OmSegmentValidateActionImpl() {}

    OmSegmentValidateActionImpl(const SegmentationDataWrapper& sdw,
                                om::shared_ptr<std::set<OmSegment*> > selectedSegments,
                                const bool valid)
        : sdw_(sdw)
        , valid_(valid)
        , selectedSegments_(selectedSegments)
    {}

    void Execute()
    {
        OmSegmentValidation::SetAsValidated(sdw_,
                                            selectedSegments_,
                                            valid_);
        OmCacheManager::TouchFreshness();
    }

    void Undo()
    {
        OmSegmentValidation::SetAsValidated(sdw_,
                                            selectedSegments_,
                                            !valid_);
        OmCacheManager::TouchFreshness();
    }

    std::string Description() const
    {
        QString lineItem;
        if(valid_) {
            lineItem = QString("Validated: ");
        } else {
            lineItem = QString("Invalidated: ");
        }

        int count = 0;
        FOR_EACH(iter, *selectedSegments_)
        {
            lineItem += QString("seg %1 + ").arg((*iter)->value());
            if(count > 10){
                break;
            }
            count++;
        }

        return lineItem.toStdString();
    }

    QString classNameForLogFile() const {
        return "OmSegmentValidateAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;

    friend QDataStream &operator<<(QDataStream&, const OmSegmentValidateActionImpl&);
    friend QDataStream &operator>>(QDataStream&, OmSegmentValidateActionImpl&);
};

#endif
