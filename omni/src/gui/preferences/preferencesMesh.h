#ifndef PREFERENCESMESH_H
#define PREFERENCESMESH_H

#include <QtGui>

class PreferencesMesh : public QWidget
{
    Q_OBJECT

public:
    PreferencesMesh(QWidget *parent);

private Q_SLOTS:
    void on_tdpSlider_valueChanged();	// om::PREF_MESH_REDUCTION_PERCENT_FLT
    void on_pasSlider_valueChanged();	// om::PREF_MESH_PRESERVED_SHARP_ANGLE_FLT
    void on_nsSlider_valueChanged();	// om::PREF_MESH_NUM_SMOOTHING_ITERS_INT

private:
    QGroupBox* makeSmoothnessBox();
    QSlider *tdpSlider;
    QLabel *tdpSliderLabel;

    QGroupBox* makeSharpnessBox();
    QSlider *pasSlider;
    QLabel *pasSliderLabel;

    QGroupBox* makeDecimationBox();
    QSlider *nsSlider;
    QLabel *nsSliderLabel;

    QGroupBox* makeBoxGeneric( QLabel** label, QSlider** slider, QString title );
};
#endif
