#ifndef OM_WIDGET_HPP
#define OM_WIDGET_HPP

#include <QtGui>

class OmWidget : public QWidget {
public:
    OmWidget(QWidget* p)
        : QWidget(p)
    {}

    virtual QString getName() = 0;
};

#endif
