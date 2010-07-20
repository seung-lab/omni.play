#ifndef RECENT_FILE_LIST_H
#define RECENT_FILE_LIST_H

#include <QAction>
#include <QStringList>

// TODO: make into preference (purcaro)
enum { MAX_NUMBER_OF_RECENT_FILES = 5 };

class RecentFileList 
{
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

#endif
