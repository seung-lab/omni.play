#ifndef VOLUMEINSPECTOR_H
#define VOLUMEINSPECTOR_H

#include <QtGui>
#include <QWidget> 
#include <QComboBox>

#include "common/omCommon.h"
#include "project/omProject.h"

class VolumeInspector : public QWidget
{ 
    Q_OBJECT 
	
public: 
	VolumeInspector(QWidget *parent); 
	QPushButton *addChannelButton;
	QPushButton *addSegmentationButton;
		
        OmVolume & GetVolume() {
                if (mVolumeType == CHANNEL) {
                        OmChannel & current_channel = OmProject::GetChannel(mImageId);
                        return current_channel;
                } else {
                        OmSegmentation & current_seg = OmProject::GetSegmentation(mImageId);
                        return current_seg;
                }
        }

private slots: 
	void on_nameEdit_editingFinished();
	void on_scaleEdit_editingFinished();
	void on_resolutionEdit_editingFinished();
	void on_extentEdit_editingFinished();
	void on_notesEdit_textChanged();
	void on_sizeSlider_valueChanged();
	void on_unitList_activated();	
 private:
	OmId mImageId;
	ObjectType mVolumeType;
	QLabel* sizeLabel;
	QLabel* dimSizeLabel;
	QLineEdit *nameEdit;
	QLineEdit *scaleEdit;
	QLineEdit *resolutionEdit;
	QLineEdit *extentEdit;
	QComboBox *unitList;
	QSlider *sizeSlider;
	QPlainTextEdit *notesEdit;

	QGroupBox* makeNotesBox();
	QGroupBox* makeVolPropBox();
	QGroupBox* makeSrcPropBox();
	void init_values();
	void SizeLabelUpdate();
}; 
#endif
