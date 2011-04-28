#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "chanInspector.h"
#include "common/omDebug.h"
#include "common/omStd.h"
#include "utility/dataWrappers.h"
#include "utility/sortHelpers.h"
#include "volInspector.h"
#include "volume/build/omBuildChannel.hpp"

#include <QtGui>

ChanInspector::ChanInspector(ChannelDataWrapper incoming_cdw, QWidget * parent)
    : QWidget(parent)
{
    setupUi(this);

    cdw = om::make_shared<ChannelDataWrapper>(incoming_cdw);
    directoryEdit->setReadOnly(true);

    populateChannelInspector();

    // connect(nameEdit, SIGNAL(editingFinished()),
    // 	this, SLOT(nameEditChanged()), Qt::QueuedConnection);
}

ChannelDataWrapper ChanInspector::getChannelDataWrapper()
{
    return *cdw;
}

void ChanInspector::on_nameEdit_editingFinished()
{
    cdw->GetChannel().SetCustomName(nameEdit->text());
}

void ChanInspector::on_browseButton_clicked()
{
    const QString dir =
        QFileDialog::getExistingDirectory(this, "Choose Directory",
                                          "", QFileDialog::ShowDirsOnly);
    if (dir != "") {
        directoryEdit->setText(dir);
        updateFileList();
    }
}

void ChanInspector::on_exportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export As"));
    if( NULL == fileName ){
        return;
    }

    OmExportVolToHdf5::Export(cdw->GetChannel(), fileName);
}

QDir ChanInspector::getDir()
{
    QString regex = patternEdit->text();
    QDir dir( directoryEdit->text() );

    dir.setSorting( QDir::Name );
    dir.setFilter( QDir::Files );
    QStringList filters;
    filters << regex;
    dir.setNameFilters( filters );

    return dir;
}

QStringList ChanInspector::getFileList()
{
    QStringList files = getDir().entryList();
    return SortHelpers::SortNaturally( files );
}

QFileInfoList ChanInspector::getFileInfoList()
{
    QFileInfoList files = getDir().entryInfoList();
    return SortHelpers::SortNaturally( files );
}

void ChanInspector::updateFileList()
{
    listWidget->clear();

    const QStringList files = getFileList();
    FOR_EACH(iter, files){
        listWidget->addItem(*iter);
    }

    listWidget->update();
}

void ChanInspector::on_patternEdit_textChanged()
{
    updateFileList();
}

void ChanInspector::on_buildButton_clicked()
{
    OmChannel & current_channel = cdw->GetChannel();

    OmBuildChannel* bc = new OmBuildChannel(&current_channel);
    bc->setFileNamesAndPaths( getFileInfoList() );
    bc->BuildNonBlocking();
}

void ChanInspector::on_notesEdit_textChanged()
{
    cdw->GetChannel().SetNote(notesEdit->toPlainText());
}

OmID ChanInspector::getChannelID()
{
    return cdw->getID();
}

void ChanInspector::populateChannelInspector()
{
    nameEdit->setText( cdw->getName() );
    nameEdit->setMinimumWidth(200);

    //TODO: fix me!
    if( 0 ){
        // use path from where import files were orginally...
    } else {
        const QString folder = QFileInfo(OmProject::OmniFile()).absolutePath();
        directoryEdit->setText(folder);
    }
    directoryEdit->setMinimumWidth(200);

    patternEdit->setText( "*" );
    patternEdit->setMinimumWidth(200);


    OmChannel& current_channel = cdw->GetChannel();
    gridLayout_3->addWidget(new OmVolInspector(current_channel, this), 4, 0, 1, 1);

    notesEdit->setPlainText( cdw->getNote() );

    updateFileList();
}

void ChanInspector::nameEditChanged()
{
    //	sdw.setName( nameEdit->text() );
}
