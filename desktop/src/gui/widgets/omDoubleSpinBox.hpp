#pragma once

#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/widgets/omCursors.h"
#include "system/omConnect.hpp"

#include <QtGui>

class OmDoubleSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    OmDoubleSpinBox(QWidget* d,
                    const om::common::ShouldUpdateAsType updateAsType)
        : QDoubleSpinBox(d)
    {
        if(om::common::UPDATE_AS_TYPE == updateAsType)
        {
            om::connect(this, SIGNAL(valueChanged(double)),
                        this, SLOT(valueChanged()));

        } else {
            om::connect(this, SIGNAL(editingFinished()),
                        this, SLOT(valueChanged()));
        }
    }

private Q_SLOTS:
    void valueChanged()
    {
        actUponValueChange(getGUIvalue());
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }

protected:
    double getGUIvalue(){
        return value();
    }

    void setGUIvalue(const double newThreshold){
        setValue(newThreshold);
    }

    virtual void setInitialGUIThresholdValue() {};
    virtual void actUponValueChange(const double threshold) = 0;
};

