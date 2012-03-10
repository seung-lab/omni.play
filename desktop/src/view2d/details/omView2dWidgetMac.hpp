#pragma once

class OmView2dWidgetBase : public QWidget {
public:
    virtual void Initialize() = 0;
    virtual void Paint() = 0;
    virtual void Resize() = 0;

private:


};
