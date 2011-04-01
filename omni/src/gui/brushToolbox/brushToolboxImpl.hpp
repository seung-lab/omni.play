#ifndef BRUSH_TOOLBOX_IMPL_HPP
#define BRUSH_TOOLBOX_IMPL_HPP

#include "system/omStateManager.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "viewGroup/omBrushSize.hpp"

#include <limits>

class OmBrushSizeSpinBox : public OmIntSpinBox {
private:
    virtual void setInitialGUIThresholdValue(){
        setValue(OmStateManager::BrushSize()->Diameter());
    }

    virtual void actUponSpinboxChange(const int value)
    {
        OmStateManager::BrushSize()->SetDiameter(value);
        OmEvents::Redraw2d();
    }

public:
    OmBrushSizeSpinBox(QWidget* parent)
        : OmIntSpinBox(parent, om::UPDATE_AS_TYPE)
    {
        setValue(OmStateManager::BrushSize()->Diameter());
        setSingleStep(1);
        setMinimum(1);
        setMaximum(std::numeric_limits<int32_t>::max());

        om::connect(OmStateManager::BrushSize(), SIGNAL(SignalBrushSizeChange(int)),
                    this, SLOT(setValue(int)));
    }
};

class BrushToolboxImpl : public QDialog {
private:
    OmBrushSizeSpinBox *const spinbox_;

public:
    BrushToolboxImpl(QWidget* parent)
        : QDialog(parent, Qt::Tool)
        , spinbox_(new OmBrushSizeSpinBox(this))
    {
        setAttribute(Qt::WA_ShowWithoutActivating);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(spinbox_);
        setLayout(layout);
    }
};

#endif
