#ifndef CHANINSPECTOR_H
#define CHANINSPECTOR_H

#include <QWidget> 
#include "ui_chanInspector.h" 
#include "system/omSystemTypes.h"
#include "utility/dataWrappers.h"
#include "volume/omChannel.h"
#include <QFileInfoList>
#include <QStringList>

class ChanInspector : public QWidget, public Ui::chanInspector
{ 
	Q_OBJECT 
	
 public: 
	ChanInspector( ChannelDataWrapper incoming_cdw, QWidget *parent);
 	QString raiseFileDialog();
 	void intermediate_build_call(OmChannel *current_channel);
	ChannelDataWrapper getChannelDataWrapper();

 private slots:
	void on_nameEdit_editingFinished();
	void on_browseButton_clicked();
	void on_patternEdit_editingFinished();
	void on_patternEdit_textChanged();
	void on_buildButton_clicked();
	void on_notesEdit_textChanged();
	void on_exportButton_clicked();
    
 private:
	ChannelDataWrapper cdw;
	void updateFileList();
	void populateChannelInspector();

	QDir getDir();
	QStringList getFileList();
	QFileInfoList getFileInfoList();
}; 
#endif
