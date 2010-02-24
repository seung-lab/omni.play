#ifndef VOLUMEINSPECTOR_H
#define VOLUMEINSPECTOR_H

#include <QtGui>
#include <QWidget> 
#include <QComboBox>

class VolumeInspector : public QWidget
{ 
    Q_OBJECT 
	
public: 
	VolumeInspector(QWidget *parent = 0); 
	QPushButton *addChannelButton;
	QPushButton *addSegmentationButton;
		
private slots: 
	void on_nameEdit_editingFinished();
	void on_scaleEdit_editingFinished();
	void on_resolutionEdit_editingFinished();
	void on_extentEdit_editingFinished();
	void on_notesEdit_textChanged();
	
	void on_sizeSlider_valueChanged();
	void on_unitList_activated();	
	void on_ramSlider_valueChanged();
	void on_vramSlider_valueChanged();

 private:
	QLabel* sizeLabel;
	QLabel* ramSizeLabel;
	QLabel* vramLabel;
	QLabel* vramSizeLabel;
	QLabel* dimSizeLabel;
	QLineEdit *nameEdit;
	QLineEdit *scaleEdit;
	QLineEdit *resolutionEdit;
	QLineEdit *extentEdit;
	QComboBox *unitList;
	QSlider *sizeSlider;
	QSlider *ramSlider;
	QSlider *vramSlider;
	QPlainTextEdit *notesEdit;

	QGroupBox* makeNotesBox();
	//	QGroupBox* makeAddDataBox();
	QGroupBox* makeCachePropBox();
	QGroupBox* makeVolPropBox();
	QGroupBox* makeSrcPropBox();
	void init_values();
	void SizeLabelUpdate();
}; 
#endif
