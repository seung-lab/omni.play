#ifndef LOCAL_PREFERENCES_SYSTEM_H
#define LOCAL_PREFERENCES_SYSTEM_H

#include <QtGui>

class LocalPreferencesSystem : public QWidget
{ 
    Q_OBJECT 
	
public: 
	LocalPreferencesSystem(QWidget *parent = 0); 
    
 private slots: 
	void on_numThreadsSlider_valueChanged();
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();

	void on_numThreadsSlider_sliderReleased();
	void on_ramSlider_sliderReleased();
	void on_vramSlider_sliderReleased();

	void on_storeMeshesInTempFolder_stateChanged( int state);

 private:
	QGroupBox * makeNumberOfThreadsBox();
	QSlider * numThreadsSlider;
        QLabel * numThreadsSliderLabel;

	QGroupBox* makeCachePropBox();
	void init_cache_prop_values();
	QLabel * sizeLabel;
	QLabel * ramSizeLabel;
	QLabel * vramLabel;
	QLabel * vramSizeLabel;
	QSlider * ramSlider;
	QSlider * vramSlider;

	QGroupBox* makeMeshBox();
        QCheckBox * storeMeshesInTempFolder;
}; 
#endif
