#include "gui/recentFileList.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include <iostream>
#include "system/omDebug.h"
using namespace std;

/********************************************
/* helper class to track recently created/opened files
/* TODO:     store recently-used files in a YAML configuration file?
/* BASED ON: http://doc.trolltech.com/4.3/mainwindows-recentfiles-mainwindow-cpp.html
/* ADDED BY: purcaro
/********************************************/

RecentFileList::RecentFileList()
{
}

RecentFileList::~RecentFileList()
{
}

QStringList RecentFileList::getRecentFiles()
{
	QStringList ret;
	for( int i = 0; i < getNumberOfFilesToShow(); i++ ){
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

	if( curNumFiles > maxNumFiles ){
		return maxNumFiles;
	} 
	else {
		return curNumFiles;
	}
}

void RecentFileList::addFile( QString fileName, QString pathName )
{
	QString rel_fnpn =  pathName + fileName;
	QFileInfo fInfo(rel_fnpn);
	QString fnpn = fInfo.absoluteFilePath();

	// remove duplicate file name
	if( recentFiles.contains( fnpn ) ){
		recentFiles.removeAt( recentFiles.indexOf( fnpn ) );
	}

	recentFiles.prepend( fnpn );
	writeRecentlyUsedFileListToFS();
	updateRecentFilesMenu();
}

void RecentFileList::updateRecentFilesMenu()
{
	QStringList recentFilesList = getRecentFiles();

	for( int i = 0; i < getNumberOfFilesToShow(); i++ ){
	    	QString fileNameAndPath = recentFilesList.at(i);
		recentFileActs[i]->setText( fileNameAndPath );
		recentFileActs[i]->setData( fileNameAndPath );
		recentFileActs[i]->setVisible( true );
	}
}

QString RecentFileList::getRecentlyUsedFilesName()
{
	char *homeStr = getenv("HOME");
	if( NULL == homeStr ){ //TODO: is there a better way to handle this error?
		cout << "could not get HOME folder path\n";
		return QString("");
	}
	return QString(homeStr) + "/.omni/recentlyOpenedFiles.txt";
}

void RecentFileList::loadRecentlyUsedFilesListFromFS()
{
	QString fname = getRecentlyUsedFilesName();
	
	if( !QFile::exists( fname ) ) {
		cout << "could not find most recently-used files list at \"" 
			<< qPrintable(getRecentlyUsedFilesName()) << "\"\n";
		return;
	}
	
	QFile file( fname );
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		cout << "could not read most recently-used files list at \"" 
			<< qPrintable( getRecentlyUsedFilesName()) << "\"\n";
		return;
	}
	
	QTextStream in(&file);
	while( !in.atEnd() ) {
		QString line = in.readLine();
		recentFiles.append( line );
	}
	
	updateRecentFilesMenu();
}

void RecentFileList::writeRecentlyUsedFileListToFS()
{
	QFile file( getRecentlyUsedFilesName() );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
		cout << "could not write most recently-used files list at \"" 
			<< qPrintable( getRecentlyUsedFilesName() ) << "\"\n";
		return;
	}
	
	QTextStream out(&file);
	for( int i = 0; i < getNumberOfFilesToShow(); i++ ){
		out << recentFiles.at(i) << endl;
	}
}
