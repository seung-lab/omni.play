#ifndef OM_DATA_ARCHIVE_PROJECT_H
#define OM_DATA_ARCHIVE_PROJECT_H

#include <QString>

class OmProjectImpl;

class OmDataArchiveProject {
public:
    static void ArchiveRead(const QString& fnp, OmProjectImpl* project);
    static void ArchiveWrite(const QString& fnp, OmProjectImpl* project);

private:
    static void upgrade();
    static void postLoad();
};

#endif
