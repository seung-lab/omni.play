#pragma once

#include "common/string.hpp"
#include "common/common.h"
#include "zi/omUtility.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

class LocalPrefFilesImpl {
public:
    inline bool settingExists(const QString& setting)
    {
        if(prefFolder_.exists(getFileName(setting))){
            return true;
        }
        return false;
    }

// bool
    inline bool readSettingBool(const QString& setting)
    {
        if(readSettingNumber<uint32_t>(setting)){
            return true;
        }
        return false;
    }

    inline void writeSettingBool(const QString& setting, const bool value){
        writeSettingNumber<uint32_t>(setting, value);
    }

// number
    template <typename T>
    inline T readSettingNumber(const QString& setting)
    {
        QStringList lines = readFile(setting);
        if(0 == lines.size()){
            throw om::IoException("invalid preference found", setting.toStdString());
        }

        return om::string::toNum<T>(lines[0].toStdString());
    }

    template <typename T>
    inline void writeSettingNumber(const QString& setting, const T value)
    {
        const QString fnp = getFileName(setting);

        QFile file(fnp);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw om::IoException("could not write file", fnp.toStdString());
        }

        QTextStream out(&file);
        out << QString::number(value);
    }

// QString
    inline QString readSettingQString(const QString& setting)
    {
        const QStringList lines = readFile(setting);
        if(1 != lines.size()){
            throw om::IoException("no preference found", setting.toStdString());
        }

        return lines[0];
    }

    inline void writeSettingQString(const QString& setting,
                                    const QString& value)
    {
        const QString fnp = getFileName(setting);

        QFile file(fnp);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw om::IoException("could not write file", fnp.toStdString());
        }

        QTextStream out(&file);
        out << value << endl;
    }

// QStringList
    inline QStringList readSettingQStringList(const QString& setting){
        return readFile(setting);
    }

    inline void writeSettingQStringList(const QString& setting,
                                        const QStringList& values)
    {
        const QString fnp = getFileName(setting);

        QFile file(fnp);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            throw om::IoException("could not write file", fnp.toStdString());
        }

        QTextStream out(&file);
        FOR_EACH(iter, values){
            out << *iter << endl;
        }
    }

private:

    QDir prefFolder_;

    LocalPrefFilesImpl() {
        setupPrefFolder();
    }

    inline void setupPrefFolder()
    {
        const QString omniFolderName = ".omni";
        const QString homeFolder = QDir::homePath();
        const QString omniFolderPath = homeFolder + "/" + omniFolderName;

        QDir dir = QDir(omniFolderPath);
        if(dir.exists()){
            prefFolder_ = dir;
            return;
        }

        if(QDir::home().mkdir(omniFolderName)){
            printf("made folder %s\n", qPrintable(omniFolderPath));
            prefFolder_ = dir;
        } else {
            throw om::IoException("could not create folder", omniFolderPath.toStdString());
        }
    }

    inline QString getFileName(const QString& setting){
        return prefFolder_.filePath(setting + ".txt");
    }

    inline QStringList readFile(const QString& setting)
    {
        const QString fnp = getFileName(setting);

        QFile file(fnp);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw om::IoException("could not read file", fnp.toStdString());
        }

        QStringList lines;
        QTextStream in(&file);
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }

        return lines;
    }

    friend class LocalPrefFiles;
};

