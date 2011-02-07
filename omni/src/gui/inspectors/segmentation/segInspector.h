#ifndef SEG_INSPECTOR_H
#define SEG_INSPECTOR_H

#include <QtGui>

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

class AddSegmentButton;
class OmSegmentation;
class MyInspectorWidget;
class OmViewGroupState;

class SegInspector : public QWidget {
Q_OBJECT

public:
    SegInspector(const SegmentationDataWrapper& sdw,
                 MyInspectorWidget* parent);

    const SegmentationDataWrapper& GetSegmentationDataWrapper() const {
        return sdw_;
    }

    QLineEdit * nameEdit;
    QLabel *directoryLabel;
    QListWidget* listWidget;
    QLineEdit *patternEdit;
    QComboBox *buildComboBox;
    QPlainTextEdit *notesEdit;
    QLineEdit *directoryEdit ;

    void rebuildSegmentLists(const OmID segmentationID, const OmSegID segID);

    OmViewGroupState* getViewGroupState();

private Q_SLOTS:
    void on_nameEdit_editingFinished();
    void on_browseButton_clicked();
    void on_patternEdit_textChanged();
    void on_notesEdit_textChanged();

Q_SIGNALS:
    void meshBuilt(OmID seg_id);

private:
    SegmentationDataWrapper sdw_;
    MyInspectorWidget *const inspectorWidget_;

    void populateSegmentationInspector();
    void updateFileList();
    void showMeshPreviewer();

    QGroupBox* makeActionsBox();
    QGroupBox* makeSourcesBox();
    QGroupBox* makeToolsBox();
    QGroupBox* makeNotesBox();
    QGroupBox* makeStatsBox();
    QGroupBox* makeVolBox();

    QDir getDir();
    QStringList getFileList();
    QFileInfoList getFileInfoList();

    AddSegmentButton *addSegmentButton;

    friend class BuildButton;
};
#endif
