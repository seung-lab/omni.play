#ifndef SEG_INSPECTOR_H
#define SEG_INSPECTOR_H

#include <QtGui>

#include "common/omCommon.h"

class AddSegmentButton;
class OmSegmentation;
class MyInspectorWidget;
class SegmentationDataWrapper;

class SegInspector : public QWidget
{
    Q_OBJECT

public:
	SegInspector( const SegmentationDataWrapper sdw, MyInspectorWidget* parent);

	OmId getSegmentationID();
	SegmentationDataWrapper getSegmentationDataWrapper();

	QLineEdit * nameEdit;
	QLabel *directoryLabel;
	QListWidget* listWidget;
	QLineEdit *patternEdit;
	QComboBox *buildComboBox;
	QPlainTextEdit *notesEdit;
	QLineEdit *directoryEdit ;

	void rebuildSegmentLists(const OmId segmentationID, const OmSegID segID);

 private slots:
	void on_nameEdit_editingFinished();
	void on_browseButton_clicked();
	void on_patternEdit_textChanged();
	void on_buildButton_clicked();
	void on_notesEdit_textChanged();
	void on_exportButton_clicked();

 signals:
	void meshBuilt(OmId seg_id);

 private:
	boost::shared_ptr<SegmentationDataWrapper> sdw;
	MyInspectorWidget *const mParent;

	void populateSegmentationInspector();
	void updateFileList();

	QGroupBox* makeActionsBox();
	QGroupBox* makeSourcesBox();
	QGroupBox* makeToolsBox();
	QGroupBox* makeNotesBox();
	QGroupBox* makeStatsBox();
	QGroupBox* makeVolBox();

	void doMeshinate( OmSegmentation * current_seg );
	QProcess* mMeshinatorProc;
	QDialog* mMeshinatorDialog;

	QDir getDir();
	QStringList getFileList();
	QFileInfoList getFileInfoList();

	AddSegmentButton *addSegmentButton;
};
#endif
