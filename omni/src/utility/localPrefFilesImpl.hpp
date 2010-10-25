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
			throw OmIoException("no preference found");
		}

		bool ok;
		int32_t ret = lines[0].toInt(&ok, 10);

		if(!ok){
			throw OmIoException("could not parse preference");
		}

		return ret;
	}
	inline void writeSettingInt(const QString& setting, const int32_t value)
	{
		QFile file(getFileName(setting));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file");
		}

		QTextStream out(&file);
		out << QString::number(value);
	}

	inline uint32_t readSettingUInt(const QString& setting)
	{
		QStringList lines = readFile(setting);
		if(0 == lines.size()){
			throw OmIoException("no preference found");
		}

		bool ok;
		uint32_t ret = lines[0].toUInt(&ok, 10);

		if(!ok){
			throw OmIoException("could not parse preference");
		}

		return ret;
	}
	inline void writeSettingUInt(const QString& setting, const uint32_t value)
	{
		QFile file(getFileName(setting));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file");
		}

		QTextStream out(&file);
		out << QString::number(value);
	}

	inline float readSettingFloat(const QString& setting)
	{
		QStringList lines = readFile(setting);
		if(0 == lines.size()){
			throw OmIoException("no preference found");
		}

		bool ok;
		float ret = lines[0].toFloat(&ok);

		if(!ok){
			throw OmIoException("could not parse preference");
		}

		return ret;
	}
	inline void writeSettingFloat(const QString& setting, const float value)
	{
		QFile file(getFileName(setting));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file");
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
		QFile file(getFileName(setting));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file");
		}

		QTextStream out(&file);
		FOR_EACH(iter, values){
			out << *iter << endl;
		}
	}

	inline void writeSettingQString(const QString& setting,
									const QString& value)
	{
        QFile file(getFileName(setting));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			throw OmIoException("could not write file");
        }

        QTextStream out(&file);
		out << value << endl;
	}
	inline QString readSettingQString(const QString& setting)
	{
        QStringList lines = readFile(setting);
        if(1 != lines.size()){
			throw OmIoException("no preference found");
        }

        return lines[0];
	}

	inline bool readSettingBool(const QString& setting)
	{
		const unsigned int val = readSettingUInt(setting);
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
		QString omniFolderName = ".omni";
		QString homeFolder = QDir::homePath();
		QString omniFolderPath = homeFolder + "/" + omniFolderName;

		QDir dir = QDir(omniFolderPath);
		if(dir.exists()){
			prefFolder_ = dir;
			return;
		}

		if(QDir::home().mkdir(omniFolderName)){
			printf("made folder %s\n", qPrintable(omniFolderPath));
			prefFolder_ = dir;
		} else {
			printf("could not make folder %s\n", qPrintable(omniFolderPath));
			throw OmIoException("could not create .omni");
		}
	}

	inline QString getFileName(const QString& setting)
	{
		return prefFolder_.filePath(setting + QString(".txt"));
	}

	inline QStringList readFile(const QString& setting)
	{
		QFile file(getFileName(setting));
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			throw OmIoException("could not read file");
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
