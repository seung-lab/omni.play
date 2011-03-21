#ifndef SEGOBJECTINSPECTOR_H
#define SEGOBJECTINSPECTOR_H

#include "utility/dataWrappers.h"

#include <QtGui>
#include <QWidget>

class SegObjectInspector : public QWidget
{
    Q_OBJECT

public:
    SegObjectInspector(const SegmentDataWrapper& sdw, QWidget* parent);

private Q_SLOTS:
    void setSegObjColor();
    void nameEditChanged();

private:
    const SegmentDataWrapper sdw_;

    QColor current_color;

    QLineEdit* nameEdit;
    QLineEdit* segmentIDEdit;
    QLineEdit* tagsEdit;
    QPushButton* colorButton;
    QPlainTextEdit* notesEdit;
    QLabel* sizeWithChildren;
    QLabel* sizeNoChildren;
    QLabel* listType_;
    QLabel* origDataValueList;
    QLabel* chunkList;

    QGroupBox* makeSourcesBox();
    QGroupBox* makeNotesBox();
    void set_initial_values();
};
#endif
