#ifndef LOCAL_PREFERENCES_H
#define LOCAL_PREFERENCES_H

#include <QtGui>

class LocalPreferences : public QWidget
{ 
    Q_OBJECT 
	
public: 
	LocalPreferences(QWidget *parent = 0); 
    
 private slots: 
	void on_numThreadsSlider_valueChanged();
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();

	void on_numThreadsSlider_sliderReleased();
	void on_ramSlider_sliderReleased();
	void on_vramSlider_sliderReleased();

 private:
	QGroupBox* makeNumberOfThreadsBox();
	QSlider *numThreadsSlider;
        QLabel  *numThreadsSliderLabel;	

	QGroupBox* makeCachePropBox();
	void init_cache_prop_values();
	QLabel* sizeLabel;
	QLabel* ramSizeLabel;
	QLabel* vramLabel;
	QLabel* vramSizeLabel;
	QSlider *ramSlider;
	QSlider *vramSlider;
}; 
#endif
