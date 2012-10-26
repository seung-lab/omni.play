#pragma once

#include "common/common.h"
#include "events/omEvents.h"
#include "project/omProject.h"
#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

#include <QSize>
#include <QSlider>

class FilterWidgetImpl : public QSlider {
Q_OBJECT

private:
    static const double delta_ = 0.1;

    bool slideAlphaForward_;

public:
    FilterWidgetImpl()
        : QSlider(Qt::Horizontal)
        , slideAlphaForward_(true)
    {
        QSize size = sizeHint();
        size.setWidth(150);
        setMaximumSize(size);

        initSilderTab();

        om::connect(this, SIGNAL(valueChanged(int)),
                    this, SLOT(setFilAlpha(int)));

        om::connect(this, SIGNAL(signalIncreaseAlpha()),
                    this, SLOT(increaseAlphaSlot()));

        om::connect(this, SIGNAL(signalDecreaseAlpha()),
                    this, SLOT(decreaseAlphaSlot()));

        om::connect(this, SIGNAL(signalCycleAlpha()),
                    this, SLOT(cycleAlphaSlot()));
    }

    void IncreaseAlpha(){
        signalIncreaseAlpha();
    }

    void DecreaseAlpha(){
        signalDecreaseAlpha();
    }

    void Cycle(){
        signalCycleAlpha();
    }

Q_SIGNALS:
    void signalIncreaseAlpha();
    void signalDecreaseAlpha();
    void signalCycleAlpha();

private Q_SLOTS:
    void increaseAlphaSlot(){
        increaseAlpha();
    }

    void decreaseAlphaSlot(){
        decreaseAlpha();
    }

    void cycleAlphaSlot(){
        cycle();
    }

private:
    void increaseAlpha()
    {
        const boost::optional<double> alpha = doGetFilterAlpha();
        if(!alpha){
            return;
        }

        double newAlpha = *alpha + delta_;

        if(newAlpha > 1){
            newAlpha = 1;
        }

        doSetFilterAlpha(newAlpha);
    }

    void decreaseAlpha()
    {
        const boost::optional<double> alpha = doGetFilterAlpha();
        if(!alpha){
            return;
        }

        double newAlpha = *alpha - delta_;

        if(newAlpha < 0){
            newAlpha = 0;
        }

        doSetFilterAlpha(newAlpha);
    }

    void cycle()
    {
        const boost::optional<double> alpha = doGetFilterAlpha();
        if(!alpha){
            return;
        }

        static const double jumpDistance = 0.6;

        double newAlpha = 0;
        if(slideAlphaForward_){
            newAlpha = jumpDistance;
            slideAlphaForward_ = false;
        } else{
            newAlpha = 0;
            slideAlphaForward_ = true;
        }

        doSetFilterAlpha(newAlpha);
    }

private Q_SLOTS:
// sliderVal is from 0 to 100
    void setFilAlpha(const int sliderVal)
    {
        const double alpha = sliderVal / 100.0;
        doSetFilterAlpha(alpha);
    }

private:
    OmID getChannelID()
    {
        return 1;
    }

    OmID getFilterID()
    {
        return 1;
    }

    void initSilderTab()
    {
        const boost::optional<double> alpha = doGetFilterAlpha();

        if(alpha){
            moveSliderTab(*alpha);
        }
    }

    void moveSliderTab(const double alpha)
    {
        this->setValue(alpha * 100);
        OmEvents::Redraw2d();
    }

    boost::optional<double> doGetFilterAlpha()
    {
        FilterDataWrapper fdw(getChannelID(), getFilterID());

        if(fdw.isValid()){
            OmFilter2d* filter = fdw.getFilter();
            return boost::optional<double>(filter->GetAlpha());
        }

        return boost::optional<double>();
    }

    void doSetFilterAlpha(const double alpha)
    {
        FilterDataWrapper fdw(getChannelID(), getFilterID());

        if(fdw.isValid()){
            OmFilter2d* filter = fdw.getFilter();
            filter->SetAlpha(alpha);
            moveSliderTab(alpha);
        }
    }
};

