#pragma once

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
    GraphTools(DendToolBar*, OmViewGroupState* vgs);

    void SetSplittingOff();
    void SetCuttingOff();
    void RefreshThreshold();

    OmViewGroupState* GetViewGroupState() const {
        return vgs_;
    }

    SegmentationDataWrapper GetSDW();
    void updateGui();

    void HideBreakThreasholdBox();
    void ShowBreakThreasholdBox();

    QString getName(){
        return "Graph Tools";
    }

private Q_SLOTS:
    void setSplittingOff();
    void setCutButtonOff();

Q_SIGNALS:
    void signalSplittingOff();
    void signalSetCutButtonOff();

private:
    DendToolBar *const mParent;
    OmViewGroupState *const vgs_;

    SplitButton* splitButton;
    CutButton* cutButton;

    ThresholdGroup* threshold_;

    QWidget* thresholdBox();
    QWidget* makeBreakThresholdBox();
    QWidget* breakThresholdBox_;
};

