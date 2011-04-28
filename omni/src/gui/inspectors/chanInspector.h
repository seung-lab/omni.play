#ifndef CHANINSPECTOR_H
#define CHANINSPECTOR_H

#include "ui_chanInspector.h"
#include "volume/omChannel.h"

#include <QWidget>
#include <QFileInfoList>
#include <QStringList>

class ChannelDataWrapper;

class ChanInspector : public QWidget, public Ui::chanInspector
{
    Q_OBJECT

public:
    ChanInspector( ChannelDataWrapper incoming_cdw, QWidget *parent);

    OmID getChannelID();

    QString raiseFileDialog();
    ChannelDataWrapper getChannelDataWrapper();

private Q_SLOTS:
    void on_nameEdit_editingFinished();
    void on_browseButton_clicked();
    void on_patternEdit_textChanged();
    void on_buildButton_clicked();
    void on_notesEdit_textChanged();
    void on_exportButton_clicked();
    void nameEditChanged();

private:
    om::shared_ptr<ChannelDataWrapper> cdw;
    void updateFileList();
    void populateChannelInspector();

    QDir getDir();
    QStringList getFileList();
    QFileInfoList getFileInfoList();
};
#endif
