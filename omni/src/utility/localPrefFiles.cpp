#include "localPrefFiles.h"

#include "common/omException.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

#include <assert.h>
#include "common/omDebug.h"

LocalPrefFiles::LocalPrefFiles()
{
	setupPrefFolder();
}

void LocalPrefFiles::setupPrefFolder()
{
	QString omniFolderName = ".omni";
	QString homeFolder = QDir::homePath();
	QString omniFolderPath = homeFolder + "/" + omniFolderName;

	QDir dir = QDir( omniFolderPath );
	if( dir.exists() ){ 
		prefFolder = dir;
		return;
	} 

	if( QDir::home().mkdir( omniFolderName ) ){
		printf("made folder %s\n", qPrintable(omniFolderPath) );
		prefFolder = dir;
	} else {
		printf("could not make folder %s\n", qPrintable(omniFolderPath));
		throw new OmIoException( "could not create .omni" );
	}
}

QString LocalPrefFiles::getFileName( QString setting )
{
	return prefFolder.filePath( setting + QString(".txt") );
}

bool LocalPrefFiles::checkIfSettingExists( QString setting )
{
	if( prefFolder.exists( getFileName(setting) ) ){
		return true;
	}
	return false;
}

QStringList LocalPrefFiles::readFile( QString setting )
{
	QFile file(getFileName(setting));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		throw new OmIoException( "could not read file" );
	}

	QStringList lines;
	QTextStream in(&file);
	while (!in.atEnd()) {
		lines.append( in.readLine() );
	}
	
	return lines;
}

int LocalPrefFiles::readSettingInt( QString setting )
{
	QStringList lines = readFile( setting );
	if( 0 == lines.size() ){
		throw new OmIoException( "no preference found" );
	}

	bool ok;
	uint ret = lines[0].toInt(&ok, 10);

	if(!ok){
		throw new OmIoException( "could not parse preference" );
	}
	
	return ret;
}

unsigned int LocalPrefFiles::readSettingUInt( QString setting )
{
	QStringList lines = readFile( setting );
	if( 0 == lines.size() ){
		throw new OmIoException( "no preference found" );
	}

	bool ok;
	uint ret = lines[0].toUInt(&ok, 10);

	if(!ok){
		throw new OmIoException( "could not parse preference" );
	}
	
	return ret;
}

void LocalPrefFiles::writeSettingUInt( QString setting, const unsigned int value )
{
	QFile file(getFileName( setting ) );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		throw new OmIoException( "could not write file" );
	}

	QTextStream out(&file);
	out << QString::number( value );
}

void LocalPrefFiles::writeSettingInt( QString setting, const int value )
{
	QFile file(getFileName( setting ) );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		throw new OmIoException( "could not write file" );
	}

	QTextStream out(&file);
	out << QString::number( value );
}
