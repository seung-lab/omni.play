#ifndef LOCAL_PREFERENCES3D_H
#define LOCAL_PREFERENCES3D_H

#include <QtGui>

class LocalPreferences3d : public QWidget
{
    Q_OBJECT

public:
    LocalPreferences3d(QWidget *parent);

private Q_SLOTS:
    void on_crosshairSlider_valueChanged();
    void on_viewSquareCheckBox_stateChanged();
    void on_viewPaneCheckBox_stateChanged();
    void on_crosshairCheckBox_stateChanged();
    void on_discoCheckBox_stateChanged();

private:
    QGroupBox* makeGeneralPropBox();
    QLabel* crosshairLabel;
    QSlider* crosshairSlider;
    QLabel* crosshairValue;
    QCheckBox* viewSquareCheckBox;
    QCheckBox* viewPaneCheckBox;
    QCheckBox* crosshairCheckBox;
    QCheckBox* discoCheckBox;
};
#endif
