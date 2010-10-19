#ifndef FILOBJECTINSPECTOR_H
#define FILOBJECTINSPECTOR_H

#include <QtGui>
#include <QWidget>
#include "project/omProject.h"

class FilterDataWrapper;

class FilObjectInspector : public QWidget
{
    Q_OBJECT

public:
    FilObjectInspector(QWidget *parent, const FilterDataWrapper & fdw);

private slots:
    void sourceEditChangedChan();
    void sourceEditChangedSeg();
    void setFilAlpha(int);
    void saveFilterAlphaValue();

private:
    QSlider *alphaSlider;

    OmID mChannelID;
    OmID mFilterID;
	boost::shared_ptr<FilterDataWrapper> mFDW;

    QLineEdit *chanEdit;
    QLineEdit *segEdit;
    QLineEdit *tagsEdit;
    QPlainTextEdit *notesEdit;

    QGroupBox* makeFilterOptionsBox();
    QGroupBox* makeNotesBox();

    void set_initial_values();
    int getSegmentationIDtoFilter();
    int getChannelIDtoFilter();
};
#endif
