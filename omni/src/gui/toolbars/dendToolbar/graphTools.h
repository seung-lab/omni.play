#ifndef GRAPH_TOOLS_H
#define GRAPH_TOOLS_H

#include "gui/widgets/omWidget.hpp"

#include <QtGui>

class CutButton;
class DendToolBar;
class OmViewGroupState;
class SegmentationDataWrapper;
class SplitButton;
class ThresholdGroup;

class GraphTools : public OmWidget {
Q_OBJECT
public:
    GraphTools(DendToolBar *);

    void ShowSplitterBusy(const bool showBusy);
    void SetSplittingOff();
    void SetCuttingOff();
    void RefreshThreshold();

    OmViewGroupState* getViewGroupState();
    SegmentationDataWrapper GetSDW();
    void updateGui();

    QString getName(){ return "Graph Tools"; }

private Q_SLOTS:
    void showSplitterBusy(const bool showBusy);
    void setSplittingOff();
    void setCutButtonOff();

Q_SIGNALS:
    void signalSplitterBusy(bool showBusy);
    void signalSplittingOff();
    void signalSetCutButtonOff();

private:
    DendToolBar *const mParent;
    SplitButton* splitButton;
    CutButton* cutButton;

    ThresholdGroup* threshold_;

    QWidget* thresholdBox();
    QWidget* breakThresholdBox();
};

#endif
