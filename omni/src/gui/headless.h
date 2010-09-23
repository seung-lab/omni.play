#ifndef HEADLESS_H
#define HEADLESS_H

#include "common/omCommon.h"

class Headless
{
 public:
	int start(int argc, char *argv[]);

 private:

	void openProject( QString fName );
	void processLine(const QString& line, const QString& fName );
	void runInteractive( QString fName );
	void runScript( const QString scriptFileName, QString fName );
	void runHeadless( QString headlessCMD, QString fName );
	void setOmniExecutablePath( QString rel_fnpn );

	void watershed(const QString &  args);

};

#endif
