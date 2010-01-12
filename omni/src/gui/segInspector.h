#ifndef SEGINSPECTOR_H
#define SEGINSPECTOR_H

#include <QWidget> 
#include "ui_segInspector.h" 
#include "system/omSystemTypes.h"
#include "volume/omSegmentation.h"

class SegInspector : public QWidget, public Ui::segInspector
{ 
    Q_OBJECT 
	
public: 
	SegInspector(OmId seg_id, QWidget *parent = 0); 
	
	void setId(OmId new_id);
	QString raiseFileDialog();
	
	void build_image(OmSegmentation *current_seg);
	void build_mesh(OmSegmentation *current_seg);	
	
	void setSegmentationID( const OmId segmenID );
	OmId getSegmentationID();
	void setSegmentID( const OmId segID );
	OmId getSegmentID();
	
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
	OmId my_id;
	OmId SegmentationID;
	OmId SegmentID;
}; 
#endif
