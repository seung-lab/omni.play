#pragma once

#include "common/omCommon.h"

class Headless {
public:
    Headless()
        : segmentationID_(0)
    {}

    void RunHeadless(const QString& headlessCMD, const QString& fName);

private:
    OmID segmentationID_;

    void processLine(const QString& line, const QString& fName);
    void runInteractive(const QString& fName);
    void runScript(const QString scriptFileName, const QString& fName);
};

