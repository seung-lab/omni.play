#ifndef LOCAL_PREFERENCES_SYSTEM_H
#define LOCAL_PREFERENCES_SYSTEM_H

#include <QtGui>

class LocalPreferencesSystem : public QWidget
{ 
    Q_OBJECT 
	
public: 
	LocalPreferencesSystem(QWidget *parent = 0); 
    
private slots: 
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();

	void on_ramSlider_sliderReleased();
	void on_vramSlider_sliderReleased();

 private:
	QGroupBox* makeCachePropBox();
	void init_cache_prop_values();
	QLabel * sizeLabel;
	QLabel * ramSizeLabel;
	QLabel * vramLabel;
	QLabel * vramSizeLabel;
	QSlider * ramSlider;
	QSlider * vramSlider;
}; 
#endif
