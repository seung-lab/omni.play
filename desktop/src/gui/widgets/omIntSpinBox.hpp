#pragma once

#include "common/om.hpp"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "system/omConnect.hpp"

#include <QtGui>

class OmIntSpinBox : public QSpinBox {
Q_OBJECT
public:
    OmIntSpinBox(QWidget * d,
                 const om::ShouldUpdateAsType updateAsType)
        : QSpinBox(d)
    {
        if(om::UPDATE_AS_TYPE == updateAsType)
        {
            om::connect(this, SIGNAL(valueChanged(int)),
                        this, SLOT(spinboxChanged()));

        } else {
            om::connect(this, SIGNAL(editingFinished()),
                        this, SLOT(spinboxChanged()) );
        }
    }

private Q_SLOTS:

    void spinboxChanged()
    {
        actUponSpinboxChange(value());
        OmEvents::Redraw2d();
    }

protected:
    virtual void setInitialGUIThresholdValue(){}
    virtual void actUponSpinboxChange(const int value) = 0;
};

