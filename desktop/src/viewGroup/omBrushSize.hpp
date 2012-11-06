#pragma once

#include "view2d/omPointsInCircle.hpp"

#include <QObject>

class OmBrushSize : public QObject {
Q_OBJECT

private:
    static const int defaultBrushDia = 8;

    OmPointsInCircle circlePts_;
    std::vector<Vector2i> ptsInBrush_;

Q_SIGNALS:
    void SignalBrushSizeChange(int);

public:
    OmBrushSize()
      : diameter_(8)
  {}

    virtual ~OmBrushSize()
    {}

    inline int Diameter() const {
        return diameter_;
    }

    void SetDiameter(const int size){
        setDiameter(size);
    }

    void IncreaseSize(){
        setDiameter(getNextSizeUp());
    }

    void IncreaseSize(const int inc){
        setDiameter(diameter_ + inc);
    }

    void DecreaseSize(){
        setDiameter(getNextSizeDown());
    }

    void DecreaseSize(const int inc)
    {
        const int newSize = diameter_ - inc;
        if(newSize < 1){
            setDiameter(1);
        } else {
            setDiameter(newSize);
        }
    }

    const std::vector<Vector2i>& GetPtsInCircle() const {
        return ptsInBrush_;
    }

private:
    int diameter_;

    void setDiameter(const int diameter)
    {
        diameter_ = diameter;
        setPtsInCircle();
        SignalBrushSizeChange(diameter_);
    }

    void setPtsInCircle(){
        ptsInBrush_ = circlePts_.GetPtsInCircle(diameter_);
    }

    int getNextSizeUp(){
        switch(diameter_){
        case 1:
            return 2;
        case 2:
            return 8;
        case 8:
            return 16;
        case 16:
            return 32;
        case 32:
        case 64:
        default:
            return 64;
        }
    }

    int getNextSizeDown(){
        switch(diameter_){
        case 1:
        case 2:
            return 1;
        case 8:
            return 2;
        case 16:
            return 8;
        case 32:
            return 16;
        case 64:
            return 32;
        default:
            return 1;
        }
    }
};

