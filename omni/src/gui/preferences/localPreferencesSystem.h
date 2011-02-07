#ifndef LOCAL_PREFERENCES_SYSTEM_H
#define LOCAL_PREFERENCES_SYSTEM_H

#include <QtGui>

class LocalPreferencesSystem : public QWidget
{
    Q_OBJECT

public:
    LocalPreferencesSystem(QWidget *parent);

private Q_SLOTS:
    void on_meshSlider_valueChanged();
    void on_tileSlider_valueChanged();

    void on_meshSlider_sliderReleased();
    void on_tileSlider_sliderReleased();

private:
    QLabel* sizeLabel;
    QLabel* meshSizeLabel;
    QLabel* vramLabel;
    QLabel* tileSizeLabel;
    QSlider* meshSlider;
    QSlider* tileSlider;

    QGroupBox* makeCachePropBox();
    void init_cache_prop_values();
};
#endif
