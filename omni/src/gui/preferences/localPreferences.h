#ifndef LOCAL_PREFERENCES_H
#define LOCAL_PREFERENCES_H

#include <QtGui>

class LocalPreferences : public QWidget
{ 
    Q_OBJECT 
	
public: 
	LocalPreferences(QWidget *parent = 0); 
    
 private slots: 
	void on_numThreads_Slider_valueChanged();
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();
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

	QGroupBox* makeBoxGeneric( QLabel** label, QSlider** slider, QString title );
}; 
#endif
