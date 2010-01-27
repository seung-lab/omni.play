#ifndef SEGOBJECTINSPECTOR_H
#define SEGOBJECTINSPECTOR_H

#include <QWidget> 
#include "ui_segObjectInspector.h" 
#include "system/omSystemTypes.h"


class SegObjectInspector : public QWidget, public Ui::segObjectInspector
{ 
    Q_OBJECT 
	
public: 
    SegObjectInspector(QWidget *parent = 0); 

    void setSegmentationID( const OmId segmenID );
    OmId getSegmentationID();
    void setSegmentID( const OmId segID );
    OmId getSegmentID();
	
    private slots: 

    private:
    OmId SegmentationID;
    OmId SegmentID;
}; 
#endif
