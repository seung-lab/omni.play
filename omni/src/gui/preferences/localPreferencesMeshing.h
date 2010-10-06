#ifndef LOCAL_PREFERENCES_MESHING_H
#define LOCAL_PREFERENCES_MESHING_H

#include <QtGui>

class LocalPreferencesMeshing : public QWidget
{
    Q_OBJECT

	public:
	LocalPreferencesMeshing(QWidget *parent);

private slots:
	void on_numThreadsSlider_valueChanged();
	void on_numThreadsSlider_sliderReleased();

private:
	QGroupBox * makeNumberOfThreadsBox();
	QSlider * numThreadsSlider;
	QLabel * numThreadsSliderLabel;

	QGroupBox* makeMeshBox();
};
#endif
