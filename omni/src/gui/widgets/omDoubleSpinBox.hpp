#ifndef OM_DOUBLE_SPIN_BOX_HPP
#define OM_DOUBLE_SPIN_BOX_HPP

#include "common/om.hpp"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "gui/widgets/omCursors.h"
#include "system/omConnect.hpp"

#include <QtGui>

class OmDoubleSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    OmDoubleSpinBox(QWidget* d,
                    const om::ShouldUpdateAsType updateAsType)
        : QDoubleSpinBox(d)
    {
        if(om::UPDATE_AS_TYPE == updateAsType)
        {
            om::connect(this, SIGNAL(valueChanged(double)),
                        this, SLOT(valueChanged()));

        } else {
            om::connect(this, SIGNAL(editingFinished()),
                        this, SLOT(valueChanged()));
        }
    }

    void updateGui(){
        OmEvents::Redraw2d();
    }

private Q_SLOTS:
    void valueChanged()
    {
        actUponValueChange(getGUIvalue());
        updateGui();
    }

protected:
    double getGUIvalue(){
        return value();
    }

    void setGUIvalue(const double newThreshold){
        setValue(newThreshold);
    }

    virtual void setInitialGUIThresholdValue() = 0;
    virtual void actUponValueChange(const double threshold) = 0;
};

#endif
