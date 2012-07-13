#pragma once

#include "gui/widgets/omWidget.hpp"

#include <QtGui>

class MSTThresholdSpinBox;
class SizeThresholdSpinBox;
class AutomaticSpreadingThresholdSpinBox;
class OmViewGroupState;
class SegmentationDataWrapper;
class SplitButton;

namespace om { namespace sidebars { class rightImpl; } }

class GraphTools : public OmWidget {
Q_OBJECT
public:
    GraphTools(om::sidebars::rightImpl*, OmViewGroupState* vgs);

    void SetSplittingOff();

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

Q_SIGNALS:
    void signalSplittingOff();

private:
    om::sidebars::rightImpl *const mParent;
    OmViewGroupState *const vgs_;

    SplitButton* splitButton;

    MSTThresholdSpinBox* threshold_;
    SizeThresholdSpinBox* sizeThreshold_;
    AutomaticSpreadingThresholdSpinBox* asthreshold_;

    QWidget* thresholdBox();
    QWidget* makeBreakThresholdBox();
    QWidget* breakThresholdBox_;
};

