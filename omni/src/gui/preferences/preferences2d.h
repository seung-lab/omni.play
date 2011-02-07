#ifndef PREFERENCES2D_H
#define PREFERENCES2D_H

#include <QWidget>
#include <QtGui>

class Preferences2d : public QWidget
{
    Q_OBJECT

public:
    Preferences2d(QWidget *parent);

private Q_SLOTS:
    void on_volEdit_editingFinished();
    void on_tileEdit_editingFinished();
    void on_depthEdit_editingFinished();
    void on_sideEdit_editingFinished();
    void on_mipEdit_editingFinished();
    void on_transparencySlider_valueChanged(int val);
    void on_infoCheckBox_stateChanged();

private:
    void set_initial_values();
    QGroupBox* makeCacheBox();
    QGroupBox* makeDisplayBox();

    QSlider *transparencySlider;
    QLabel *transparencySliderLabel;
    QCheckBox *infoCheckBox;
    QLineEdit *volEdit;
    QLineEdit *tileEdit;
    QLineEdit *depthEdit;
    QLineEdit *sideEdit;
    QLineEdit *mipEdit;
};
#endif


