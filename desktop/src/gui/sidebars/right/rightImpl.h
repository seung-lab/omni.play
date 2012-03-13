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

    SegmentationDataWrapper GetSDW();

    static bool GetShowGroupsMode();

private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    GraphTools* graphTools_;

    template <class T>
    QWidget* wrapWithGroupBox(T* widget)
    {
        QGroupBox* gbox = new QGroupBox(widget->getName(), this);
        QVBoxLayout* vbox = new QVBoxLayout(gbox);
        vbox->addWidget(widget);
        gbox->setContentsMargins(0,0,0,0);

        return gbox;
    }
};

} // namespace sidebars
} // namespace om

