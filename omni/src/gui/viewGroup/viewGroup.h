#pragma once

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
namespace om { namespace gui { class viewGroupImpl; } }

class ViewGroup {
public:
    ViewGroup(MainWindow*, OmViewGroupState*);
    ~ViewGroup();

    QDockWidget* AddView2Dchannel(const OmID chan_id, const ViewType vtype);

    std::pair<QDockWidget*,QDockWidget*>
    AddView2Dsegmentation(const OmID segmentation_id, const ViewType vtype);

    void AddView3D();
    void AddView3D4View();

    void AddAllViews(const OmID channelID, const OmID segmentationID);
    void AddXYView(const OmID channelID, const OmID segmentationID);
    void AddXYViewAndView3d(const OmID channelID, const OmID segmentationID);

private:
    MainWindow *const mainWindow_;

    boost::scoped_ptr<om::gui::viewGroupImpl> impl_;
};
