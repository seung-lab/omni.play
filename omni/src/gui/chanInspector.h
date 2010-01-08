#ifndef CHANINSPECTOR_H
#define CHANINSPECTOR_H

#include <QWidget> 
#include "ui_chanInspector.h" 
#include "system/omSystemTypes.h"

#include "volume/omChannel.h"

class ChanInspector : public QWidget, public Ui::chanInspector
{ 
    Q_OBJECT 
	
public: 
    ChanInspector(OmId chan_id, QWidget *parent = 0);
	
	void setId(OmId new_id);
	QString raiseFileDialog();
	
	void intermediate_build_call(OmChannel *current_channel);
	
	private slots:
	void on_nameEdit_editingFinished();
	void on_browseButton_clicked();
	void on_patternEdit_editingFinished();
	void on_patternEdit_textChanged();
	void on_buildButton_clicked();
	void on_notesEdit_textChanged();
	void on_exportButton_clicked();
	
private:
	OmId my_id;

}; 
#endif