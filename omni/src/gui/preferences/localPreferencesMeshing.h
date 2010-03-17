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

	void on_storeMeshesInTempFolder_stateChanged( int state);

private:
	QGroupBox * makeNumberOfThreadsBox();
	QSlider * numThreadsSlider;
        QLabel * numThreadsSliderLabel;

	QGroupBox* makeMeshBox();
        QCheckBox * storeMeshesInTempFolder;
}; 
#endif
