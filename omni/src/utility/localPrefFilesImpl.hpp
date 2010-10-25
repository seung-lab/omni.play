#ifndef LOCAL_PREF_FILES_IMPL_HPP
#define LOCAL_PREF_FILES_IMPL_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

class LocalPrefFilesImpl {
public:
	LocalPrefFilesImpl() {
		setupPrefFolder();
	}

	inline bool settingExists(const QString& setting)
	{
		if(prefFolder_.exists(getFileName(setting))){
			return true;
		}
		return false;
	}

	inline int32_t readSettingInt(const QString& setting)
	{
		QStringList lines = readFile(setting);
		if(0 == lines.size()){
			throw OmIoException("no preference found", setting);
		}

		bool ok;
		const int32_t ret = lines[0].toInt(&ok, 10);

		if(!ok){
			throw OmIoException("could not parse preference", setting);
		}

		return ret;
	}
	inline void writeSettingInt(const QString& setting, const int32_t value)
	{
		const QString fnp = getFileName(setting);

		QFile file(fnp);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file", fnp);
		}

		QTextStream out(&file);
		out << QString::number(value);
	}

	inline uint32_t readSettingUInt(const QString& setting)
	{
		QStringList lines = readFile(setting);
		if(0 == lines.size()){
			throw OmIoException("no preference found", setting);
		}

		bool ok;
		const uint32_t ret = lines[0].toUInt(&ok, 10);

		if(!ok){
			throw OmIoException("could not parse preference", setting);
		}

		return ret;
	}
	inline void writeSettingUInt(const QString& setting, const uint32_t value)
	{
		const QString fnp = getFileName(setting);

		QFile file(fnp);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file", fnp);
		}

		QTextStream out(&file);
		out << QString::number(value);
	}

	inline float readSettingFloat(const QString& setting)
	{
		QStringList lines = readFile(setting);
		if(0 == lines.size()){
			throw OmIoException("no preference found", setting);
		}

		bool ok;
		const float ret = lines[0].toFloat(&ok);

		if(!ok){
			throw OmIoException("could not parse preference", setting);
		}

		return ret;
	}
	inline void writeSettingFloat(const QString& setting, const float value)
	{
		const QString fnp = getFileName(setting);

		QFile file(fnp);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file", fnp);
		}

		QTextStream out(&file);
		out << QString::number(value);
	}

	inline QStringList readSettingQStringList(const QString& setting)
	{
		return readFile(setting);
	}
	inline void writeSettingQStringList(const QString& setting,
										const QStringList& values)
	{
		const QString fnp = getFileName(setting);

		QFile file(fnp);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file", fnp);
		}

		QTextStream out(&file);
		FOR_EACH(iter, values){
			out << *iter << endl;
		}
	}

	inline QString readSettingQString(const QString& setting)
	{
        const QStringList lines = readFile(setting);
        if(1 != lines.size()){
			throw OmIoException("no preference found", setting);
        }

        return lines[0];
	}
	inline void writeSettingQString(const QString& setting,
									const QString& value)
	{
		const QString fnp = getFileName(setting);

        QFile file(fnp);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file", fnp);
        }

        QTextStream out(&file);
		out << value << endl;
	}

	inline bool readSettingBool(const QString& setting)
	{
		const uint32_t val = readSettingUInt(setting);
		if(0 == val){
			return false;
		}

		return true;
	}
	inline void writeSettingBool(const QString& setting, const bool value)
	{
		uint32_t intValue;
		if(value){
			intValue = 1;
		} else {
			intValue = 0;
		}

		writeSettingUInt(setting, intValue);
	}

private:
	QDir prefFolder_;

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
			throw OmIoException("could not create folder", omniFolderPath);
		}
	}

	inline QString getFileName(const QString& setting)
	{
		return prefFolder_.filePath(setting + QString(".txt"));
	}

	inline QStringList readFile(const QString& setting)
	{
		const QString fnp = getFileName(setting);

		QFile file(fnp);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			throw OmIoException("could not read file", fnp);
		}

		QStringList lines;
		QTextStream in(&file);
		while (!in.atEnd()) {
			lines.append(in.readLine());
		}

		return lines;
	}
};

#endif
