#ifndef OM_NEW_FILE_DIALOG_HPP
#define OM_NEW_FILE_DIALOG_HPP

#include "datalayer/fs/omFileNames.hpp"

#include <QFileDialog>

class OmNewFileDialog {
private:
    QWidget *const parent_;
    const QString caption_;
    const QString fileNameFilter_;

public:
    OmNewFileDialog(QWidget *const parent)
        : parent_(parent)
        , caption_("New Project")
        , fileNameFilter_("Omni (*.omni)")
    {}

    /**
     * query user for new file name; if user omits ".omni" extension, and adding
     *  the extension would cause a pre-existing project to be overwritten
     *  without the user's consent, loop until the user confirms the file
     *  overwrite (or renames the file)
     **/
    QString GetNewFileName()
    {
        const QString fnp_raw = newDialog();
        if(NULL == fnp_raw){
            return NULL;
        }

        const QString fnp = OmFileNames::AddOmniExtensionIfNeeded(fnp_raw);
        if(fnp == fnp_raw){
            return fnp;
        }
        if(!QFile::exists(fnp)){
            return fnp;
        }

        return loopUntilStringsMatch(fnp);
    }

private:
    QString loopUntilStringsMatch(const QString& old)
    {
        const QString fnp = newDialog(old);
        if(NULL == fnp){
            return NULL;
        }
        if(fnp == old){
            return fnp;
        }
        if(!QFile::exists(fnp)){
            return fnp;
        }

        return loopUntilStringsMatch(fnp);
    }

    QString newDialog()
    {
        return QFileDialog::getSaveFileName(parent_,
                                            caption_,
                                            "",
                                            fileNameFilter_);
    }

    QString newDialog(const QString& fnp)
    {
        const QString str = QFileDialog::getSaveFileName(parent_,
                                                         caption_,
                                                         fnp,
                                                         fileNameFilter_);
        return OmFileNames::AddOmniExtensionIfNeeded(str);
    }
};

#endif
