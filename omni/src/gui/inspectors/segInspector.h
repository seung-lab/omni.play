#ifndef SEGINSPECTOR_H
#define SEGINSPECTOR_H

#include <QtGui>
#include <QWidget> 
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"

class SegInspector : public QWidget
{ 
    Q_OBJECT 
	
public: 
	SegInspector( const SegmentationDataWrapper sdw, QWidget *parent); 
	
	OmId getSegmentationID();

	void build_image(OmSegmentation * current_seg);
	void build_mesh(OmSegmentation * current_seg);
	void build_image_and_mesh( OmSegmentation * current_seg );
	
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
	void updateFileList();

	QGroupBox* makeActionsBox();
	QGroupBox* makeSourcesBox();
	QGroupBox* makeToolsBox();
	QGroupBox* makeNotesBox();
	QGroupBox* makeStatsBox();

	void doMeshinate( OmSegmentation * current_seg );
	QProcess* mMeshinatorProc;
	QDialog* mMeshinatorDialog;

	QDir getDir();
	QStringList getFileList();
	QFileInfoList getFileInfoList();

	QDir getDir();
	QStringList getFileList();
	QFileInfoList getFileInfoList();

}; 
#endif
