#include "common/logging.h"
#include "gui/recentFileList.h"
#include "system/omLocalPreferences.hpp"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include <iostream>

/********************************************
 * helper class to track recently created/opened files
 * BASED ON: http://doc.trolltech.com/4.3/mainwindows-recentfiles-mainwindow-cpp.html
 * ADDED BY: purcaro
 ********************************************/

RecentFileList::RecentFileList()
{}

RecentFileList::~RecentFileList()
{}

QStringList RecentFileList::getRecentFiles()
{
    QStringList ret;
    for (int i = 0; i < getNumberOfFilesToShow(); i++) {
        ret << recentFiles.at(i);
    }
    return ret;
}

// TODO: retrieve value from user preferences
int RecentFileList::getMaxNumberOfRecentlyUsedFilesToDisplay()
{
    return MAX_NUMBER_OF_RECENT_FILES;
}

int RecentFileList::getNumberOfFilesToShow()
{
    const int maxNumFiles = getMaxNumberOfRecentlyUsedFilesToDisplay();
    const int curNumFiles = recentFiles.size();

    if (curNumFiles > maxNumFiles) {
        return maxNumFiles;
    } else {
        return curNumFiles;
    }
}

void RecentFileList::addFile(QString rel_fnpn)
{
    QFileInfo fInfo(rel_fnpn);
    QString fnpn = fInfo.absoluteFilePath();

    recentFiles.removeAll( fnpn );
    recentFiles.prepend(fnpn);
    writeRecentlyUsedFileListToFS();
    updateRecentFilesMenu();
}

void RecentFileList::updateRecentFilesMenu()
{
    QStringList recentFilesList = getRecentFiles();

    for (int i = 0; i < getNumberOfFilesToShow(); i++) {
        QString fileNameAndPath = recentFilesList.at(i);
        recentFileActs[i]->setText(fileNameAndPath);
        recentFileActs[i]->setData(fileNameAndPath);
        recentFileActs[i]->setVisible(true);
    }
}

void RecentFileList::loadRecentlyUsedFilesListFromFS()
{
    QStringList allFiles = OmLocalPreferences::getRecentlyUsedFilesNames();

    for (int i = 0; i < allFiles.size(); i++) {
        recentFiles.append( allFiles.at(i) );
    }

    updateRecentFilesMenu();
}

void RecentFileList::writeRecentlyUsedFileListToFS()
{
    OmLocalPreferences::setRecentlyUsedFilesNames( recentFiles );
}

void RecentFileList::prependFileToFS( const QString & rel_fnpn)
{
    QStringList allFiles = OmLocalPreferences::getRecentlyUsedFilesNames();

    QFileInfo fInfo(rel_fnpn);
    QString fnpn = fInfo.absoluteFilePath();

    allFiles.removeAll( fnpn );
    allFiles.prepend(fnpn);

    OmLocalPreferences::setRecentlyUsedFilesNames( allFiles );
}
