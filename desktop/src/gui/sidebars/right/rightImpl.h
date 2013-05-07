#pragma once

#include "common/omCommon.h"

#include <QtGui>

class GraphTools;
class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;

namespace om {
namespace sidebars {

class rightImpl : public QWidget {
Q_OBJECT
public:
    rightImpl(MainWindow* mw, OmViewGroupState* vgs);

    void updateGui();

    void SetSplittingOff();
    void SetShatteringOff();

    SegmentationDataWrapper GetSDW();

    static bool GetShowGroupsMode();

private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    GraphTools* graphTools_;
};

} // namespace sidebars
} // namespace om

