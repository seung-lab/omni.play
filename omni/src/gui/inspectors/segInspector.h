#ifndef SEGINSPECTOR_H
#define SEGINSPECTOR_H

#include <QtGui>
#include <QWidget> 
#include "system/omSystemTypes.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"

class SegInspector : public QWidget
{ 
    Q_OBJECT 
	
public: 
	SegInspector( const SegmentationDataWrapper sdw, QWidget *parent = 0); 
	
	OmId getSegmentationID();
	QString raiseFileDialog();
	
	void build_image(OmSegmentation *current_seg);
	void build_mesh(OmSegmentation *current_seg);	
	
	QLineEdit * nameEdit;
	QLabel *directoryLabel;
	QListWidget* listWidget;
	QLineEdit *patternEdit;
	QComboBox *buildComboBox;
	QPlainTextEdit *notesEdit;
	QLineEdit *directoryEdit ;
	QPushButton *addSegmentButton;

 private slots:
	void on_nameEdit_editingFinished();
	void on_browseButton_clicked();
	void on_patternEdit_editingFinished();
	void on_patternEdit_textChanged();
	void on_buildButton_clicked();
	void on_notesEdit_textChanged();
	void on_exportButton_clicked();
		
 signals:
	void meshBuilt(OmId seg_id);
	void segmentationBuilt( OmId segmen_id );
	
 private:
	SegmentationDataWrapper sdw;
	void populateSegmentationInspector();

	QGroupBox* makeActionsBox();
	QGroupBox* makeSourcesBox();
	QGroupBox* makeToolsBox();
	QGroupBox* makeNotesBox();
	QGroupBox* makeStatsBox();

	QProcess* mMeshinatorProc;
	QDialog* mMeshinatorDialog;
}; 
#endif
