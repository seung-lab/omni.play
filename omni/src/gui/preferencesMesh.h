#ifndef PREFERENCESMESH_H
#define PREFERENCESMESH_H

#include <QSlider>
#include <QLineEdit>
#include <QWidget>
#include <QLabel>

class PreferencesMesh:public QWidget {
 Q_OBJECT public:
	PreferencesMesh(QWidget * parent = 0);

	private slots: void on_tdpSlider_valueChanged();	// OM_PREF_MESH_REDUCTION_PERCENT_FLT
	void on_pasSlider_valueChanged();	// OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT
	void on_nsSlider_valueChanged();	// OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT

 private:
	 QSlider * tdpSlider;
	QSlider *pasSlider;
	QSlider *nsSlider;
	QLineEdit *number_of_threads;

	QLabel *tdpSliderLabel;
	QLabel *pasSliderLabel;
	QLabel *nsSliderLabel;

};
#endif
