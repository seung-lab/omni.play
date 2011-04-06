#ifndef SEG_INSPECTOR_H
#define SEG_INSPECTOR_H

#include <QtGui>

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

class AddSegmentButton;
class OmSegmentation;
class OmViewGroupState;

class SegmentationInspector : public QWidget {
Q_OBJECT

public:
    SegmentationInspector(QWidget* parent, OmViewGroupState* vgs, const SegmentationDataWrapper& sdw);

    const SegmentationDataWrapper& GetSDW() const {
        return sdw_;
    }

    QLineEdit * nameEdit;
    QLabel *directoryLabel;
    QListWidget* listWidget;
    QLineEdit *patternEdit;
    QComboBox *buildComboBox;
    QPlainTextEdit *notesEdit;
    QLineEdit *directoryEdit ;

    inline OmViewGroupState* getViewGroupState(){
        return vgs_;
    }

private Q_SLOTS:
    void on_nameEdit_editingFinished();
    void on_browseButton_clicked();
    void on_patternEdit_textChanged();
    void on_notesEdit_textChanged();

Q_SIGNALS:
    void meshBuilt(OmID seg_id);

private:
    OmViewGroupState *const vgs_;
    const SegmentationDataWrapper sdw_;

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
