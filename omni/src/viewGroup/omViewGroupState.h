#ifndef OM_VIEW_GROUP_STATE_H
#define OM_VIEW_GROUP_STATE_H

#include "system/omManageableObject.h"

class FilterWidget;
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
class ToolBarManager;
class ViewGroup;

class OmViewGroupState : public OmManageableObject {
public:
    OmViewGroupState(MainWindow* mw);
    virtual ~OmViewGroupState();

    void SetInspectorProperties(InspectorProperties* ip) {
        mInspectorProperties = ip;
    }
    InspectorProperties* GetInspectorProperties() {
        return mInspectorProperties;
    }

    void SetFilterWidget(FilterWidget* f){
        mFilterWidget = f;
    }
    FilterWidget* GetFilterWidget(){
        return mFilterWidget;
    }

    // GUI state
    void addView2Dchannel(OmID chan_id, ViewType vtype);
    void addView2Dsegmentation(OmID segmentation_id, ViewType vtype);
    void addView3D();
    void AddAllViews(OmID channelID, OmID segmentationID);
    void AddXYView(const OmID channelID, const OmID segmentationID);

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

    boost::shared_ptr<OmColorRGBA> ColorTile(uint32_t const*,
                                             const Vector2i&,
                                             const OmTileCoord&);
private:
    MainWindow* mMainWindow;
    FilterWidget* mFilterWidget;
    boost::scoped_ptr<ViewGroup> mViewGroup;
    InspectorProperties* mInspectorProperties;

    boost::scoped_ptr<OmViewGroupView2dState> view2dState_;
    boost::scoped_ptr<OmColorizers> colorizers_;

    boost::scoped_ptr<OmZoomLevel> zoomLevel_;

    boost::scoped_ptr<OmSplitting> splitting_;
    boost::scoped_ptr<OmCutting> cutting_;

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
