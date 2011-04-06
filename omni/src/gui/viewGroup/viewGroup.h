#ifndef VIEW_GROUP_H
#define VIEW_GROUP_H

#include "common/omCommon.h"
#include "gui/viewGroup/viewGroupUtils.hpp"

class OmView2d;
class OmView3d;

class ViewGroup : private ViewGroupUtils {
public:
    ViewGroup(MainWindow*, OmViewGroupState*);

    QDockWidget* AddView2Dchannel(const OmID chan_id,
                                  const ViewType vtype);

    std::pair<QDockWidget*,QDockWidget*>
    AddView2Dsegmentation(const OmID segmentation_id, const ViewType vtype);

    void AddView3D();
    void AddAllViews(const OmID channelID, const OmID segmentationID);
    void AddXYView(const OmID channelID, const OmID segmentationID);

private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    QDockWidget *makeDockWidget(ViewGroupWidgetInfo&);

    std::pair<QDockWidget*,QDockWidget*> insertDockIntoGroup(ViewGroupWidgetInfo&);
    QDockWidget* insertBySplitting(ViewGroupWidgetInfo&, QDockWidget* );
    QDockWidget* insertByTabbing(ViewGroupWidgetInfo&, QDockWidget*);

    QDockWidget* chooseDockToSplit(ViewGroupWidgetInfo&);
    QDockWidget* chooseDockToTabify(ViewGroupWidgetInfo&);
};

#endif
