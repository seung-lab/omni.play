#ifndef VIEW_GROUP_H
#define VIEW_GROUP_H

#include "common/omCommon.h"

class ViewGroupUtils;
class ViewGroupWidgetInfo;
class ViewGroupMainWindowUtils;
class OmViewGroupState;

class ViewGroup {
public:
    ViewGroup(MainWindow*, OmViewGroupState*);
    ~ViewGroup();

    QDockWidget* AddView2Dchannel(const OmID chan_id,
                                  const ViewType vtype);

    std::pair<QDockWidget*,QDockWidget*>
    AddView2Dsegmentation(const OmID segmentation_id, const ViewType vtype);

    void AddView3D();
    void AddView3D4View();

    void AddAllViews(const OmID channelID, const OmID segmentationID);
    void AddXYView(const OmID channelID, const OmID segmentationID);
    void AddXYViewAndView3d(const OmID channelID, const OmID segmentationID);

private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    boost::scoped_ptr<ViewGroupUtils> utils_;
    boost::scoped_ptr<ViewGroupMainWindowUtils> mainWindowUtils_;

    void addView3D2View(QDockWidget* widget);
};

#endif
