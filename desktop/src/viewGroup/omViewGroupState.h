#pragma once

#include "system/omManageableObject.h"
#include <common/omColors.h>

class QGLWidget;
class ChannelDataWrapper;
class InspectorProperties;
class MainWindow;
class OmColorizers;
class OmLandmarks;
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
private:
    boost::scoped_ptr<ViewGroup> viewGroup_;
    boost::scoped_ptr<OmViewGroupView2dState> view2dState_;
    boost::scoped_ptr<OmColorizers> colorizers_;
    boost::scoped_ptr<OmZoomLevel> zoomLevel_;
    boost::scoped_ptr<OmSplitting> splitting_;
    boost::scoped_ptr<OmLandmarks> landmarks_;

    boost::scoped_ptr<ChannelDataWrapper> cdw_;
    boost::scoped_ptr<SegmentationDataWrapper> sdw_;

#ifdef ZI_OS_MACOS
    boost::scoped_ptr<QGLWidget> context3d_;
#endif

    float mBreakThreshold;
    uint64_t mDustThreshold;

    //toolbar stuff
    ToolBarManager* toolBarManager_;
    bool mShatter;
    bool mShowValid;
    bool mShowValidInColor;
    bool mShowFilterInColor;

    bool brightenSelected_;

    //annotation stuff
    OmColor annotationColor_;
    std::string annotationString_;
    double annotationSize_;
    bool annotationVisible_;

public:
    OmViewGroupState(MainWindow* mw);

    ~OmViewGroupState();

    ViewGroup* GetViewGroup() {
        return viewGroup_.get();
    }

    //viewbox state
    inline OmViewGroupView2dState* View2dState() {
        return view2dState_.get();
    }

    inline OmZoomLevel* ZoomLevel() {
        return zoomLevel_.get();
    }

    void setBreakThreshold(float t) {
        mBreakThreshold = t;
    }
    float getBreakThreshold() {
        return mBreakThreshold;
    }

    void setDustThreshold(uint64_t t) {
        mDustThreshold = t;
    }
    uint64_t getDustThreshold() {
        return mDustThreshold;
    }

    void SetToolBarManager(ToolBarManager* tbm);
    ToolBarManager* GetToolBarManager();

    inline bool GetShatterMode() const {
        return mShatter;
    }

    inline void ToggleShatterMode() {
        mShatter = !mShatter;
    }

    void SetShowValidMode(bool mode, bool incolor);
    bool shouldVolumeBeShownBroken();

    void setTool(const om::tool::mode tool);

    inline OmSplitting* Splitting() {
        return splitting_.get();
    }

    void SetHowNonSelectedSegmentsAreColoredInFilter(const bool);

    inline bool ShowNonSelectedSegmentsInColorInFilter() const {
        return mShowFilterInColor;
    }

    OmSegmentColorCacheType determineColorizationType(const ObjectType);

    OmPooledTile<OmColorARGB>* ColorTile(uint32_t const*const,
                                         const int tileDim,
                                         const OmTileCoord&);

    ChannelDataWrapper Channel() const;
    SegmentationDataWrapper Segmentation() const;

    void BrightenSelected(const bool brigthen) {
        brightenSelected_ = brigthen;
    }

    inline OmLandmarks& Landmarks() {
        return *landmarks_;
    }

    inline const OmColor & getAnnotationColor() {
        return annotationColor_;
    }

    inline void setAnnotationColor(const OmColor &color) {
        annotationColor_ = color;
    }

    inline const std::string & getAnnotationString() {
        return annotationString_;
    }

    inline void setAnnotationString(const std::string &string) {
        annotationString_ = string;
    }

    inline const double & getAnnotationSize() {
        return annotationSize_;
    }

    inline void setAnnotationSize(const double &size) {
        annotationSize_ = size;
    }

    inline bool getAnnotationVisible() {
        return annotationVisible_;
    }

    inline void setAnnotationVisible(bool visible) {
        annotationVisible_ = visible;
    }

#ifdef ZI_OS_MACOS
    inline QGLWidget* get3dContext() {
    	return context3d_.get();
    }
#endif
};

