#ifndef HEADLESS_H
#define HEADLESS_H

class Headless
{
 public:
	int start(int argc, char *argv[]);

 private:

	void openProject( QString fName, const bool autoOpenAndClose );
	unsigned int getNum( QString arg );
	void processLine( QString line, QString fName );
	void runInteractive( QString fName );
	void runScript( const QString scriptFileName, QString fName );
	void runHeadless( QString headlessCMD, QString fName );
	void setOmniExecutablePath( QString rel_fnpn );

};

#endif
