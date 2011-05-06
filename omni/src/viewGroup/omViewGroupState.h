#ifndef OM_VIEW_GROUP_STATE_H
#define OM_VIEW_GROUP_STATE_H

#include "system/omManageableObject.h"

class InspectorProperties;
class MainWindow;
class OmColorizers;
class OmCutting;
class OmSegmentColorizer;
class OmSplitting;
class OmTileCoord;
class OmViewGroupView2dState;
class OmZoomLevel;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class ToolBarManager;
class ViewGroup;
template <class> class OmPooledTile;

class OmViewGroupState : public OmManageableObject {
public:
    OmViewGroupState(MainWindow* mw);

    ~OmViewGroupState();

    ViewGroup* GetViewGroup(){
        return viewGroup_.get();
    }

    //viewbox state
    inline OmViewGroupView2dState* View2dState() {
        return view2dState_.get();
    }

    inline OmZoomLevel* ZoomLevel() {
        return zoomLevel_.get();
    }

    void setBreakThreshold(float t){
        mBreakThreshold = t;
    }
    float getBreakThreshold(){
        return mBreakThreshold;
    }

    void setDustThreshold(uint64_t t){
        mDustThreshold = t;
    }
    uint64_t getDustThreshold(){
        return mDustThreshold;
    }

    void SetToolBarManager(ToolBarManager* tbm);

    inline bool GetShatterMode() const {
        return mShatter;
    }

    inline void ToggleShatterMode(){
        mShatter = !mShatter;
    }

    void SetShowValidMode(bool mode, bool incolor);
    bool shouldVolumeBeShownBroken();

    void setTool(const om::tool::mode tool);

    inline OmSplitting* Splitting(){
        return splitting_.get();
    }

    inline OmCutting* Cutting(){
        return cutting_.get();
    }

    void SetHowNonSelectedSegmentsAreColoredInFilter(const bool);

    inline bool ShowNonSelectedSegmentsInColorInFilter() const {
        return mShowFilterInColor;
    }

    OmSegmentColorCacheType determineColorizationType(const ObjectType);

    OmPooledTile<OmColorARGB>* ColorTile(uint32_t const*const,
                                         const int tileDim,
                                         const OmTileCoord&);

    SegmentationDataWrapper Segmentation() const;

private:
    boost::scoped_ptr<ViewGroup> viewGroup_;
    boost::scoped_ptr<OmViewGroupView2dState> view2dState_;
    boost::scoped_ptr<OmColorizers> colorizers_;
    boost::scoped_ptr<OmZoomLevel> zoomLevel_;
    boost::scoped_ptr<OmSplitting> splitting_;
    boost::scoped_ptr<OmCutting> cutting_;

    boost::scoped_ptr<SegmentationDataWrapper> sdw_;

    float mBreakThreshold;
    uint64_t mDustThreshold;

    //toolbar stuff
    ToolBarManager* toolBarManager_;
    bool mShatter;
    bool mShowValid;
    bool mShowValidInColor;

    bool mShowFilterInColor;
};

#endif
