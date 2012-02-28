#pragma once

#include <QAction>
#include <QStringList>

class RecentFileList {
private:
    // TODO: make into preference (purcaro)
    static const int MAX_NUMBER_OF_RECENT_FILES = 5;

public:
    RecentFileList();
    ~RecentFileList();

    // QActions instantiated in mainwindow.cpp
    QAction *recentFileActs[MAX_NUMBER_OF_RECENT_FILES];

    void addFile( QString rel_fnpn );
    void loadRecentlyUsedFilesListFromFS();
    int getMaxNumberOfRecentlyUsedFilesToDisplay();

    static void prependFileToFS( const QString & rel_fnpn);

private:
    QStringList getRecentFiles();
    int getNumberOfFilesToShow();
    QStringList recentFiles;
    void updateRecentFilesMenu();
    void writeRecentlyUsedFileListToFS();
};

