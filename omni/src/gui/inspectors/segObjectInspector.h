#ifndef SEGOBJECTINSPECTOR_H
#define SEGOBJECTINSPECTOR_H

#include <QtGui>
#include <QWidget> 
#include "utility/dataWrappers.h"

class SegObjectInspector : public QWidget
{ 
    Q_OBJECT 
	
public: 
	SegObjectInspector(SegmentDataWrapper incoming_sdw, QWidget* parent);
	
private slots: 
	void setSegObjColor();
	void nameEditChanged();

private:
	SegmentDataWrapper sdw;

	QColor current_color;

	QLineEdit *nameEdit;
	QLineEdit *segmentIDEdit;
	QLineEdit *tagsEdit;
	QPushButton *colorButton;
	QPlainTextEdit *notesEdit;
	QLabel* sizeWithChildren;
	QLabel* sizeNoChildren;
	QLabel* origDataValueList;
	QLabel* chunkList;

	QGroupBox* makeSourcesBox();
	QGroupBox* makeNotesBox();
	void set_initial_values();
}; 
#endif
