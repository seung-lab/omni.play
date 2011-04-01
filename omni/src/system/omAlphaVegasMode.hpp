#ifndef OM_ALPHA_VEGA_MODE_HPP
#define OM_ALPHA_VEGA_MODE_HPP

#include "common/omDebug.h"
#include "events/details/omView3dEvent.h"
#include "gui/toolbars/mainToolbar/filterWidget.hpp"

#include <qthread.h>

class OmAlphaVegasMode: public QThread {
public:
    OmAlphaVegasMode()
        : stop_(false)
    {
        start();
    }

    ~OmAlphaVegasMode()
    {
        stop_ = true;
        wait();
    }

    void run()
    {
        Q_FOREVER{
            if(stop_){
                return;
            }

            FilterWidget::Cycle();
            QThread::msleep(666);
        }
    }

private:
    bool stop_;
};

#endif
