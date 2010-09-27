#ifndef HEADLESS_H
#define HEADLESS_H

#include "common/omCommon.h"

class Headless {
public:
	Headless() : segmentationID_(0) {}

	void runHeadless(const QString& headlessCMD, const QString& fName);

private:
	OmId segmentationID_;

	void openProject(const QString& fName);
	void processLine(const QString& line, const QString& fName);
	void runInteractive(const QString& fName);
	void runScript(const QString scriptFileName, const QString& fName);
	void watershed(const QString& args);
};

#endif
