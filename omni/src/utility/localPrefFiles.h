#ifndef LOCAL_PREF_FILES_H
#define LOCAL_PREF_FILES_H

#include "utility/localPrefFilesImpl.hpp"

class LocalPrefFiles : private om::singletonBase<LocalPrefFiles> {
public:
	static bool settingExists( QString setting)
	{
		return checkIfSettingExists( setting);
	}

	static bool readSettingBool(const QString& setting, const bool defaultRet)
	{
		if( !settingExists( setting)){
			return defaultRet;
		}

		try{
			return readSettingBool( setting );
		} catch (...) {
			return defaultRet;
		}
	}
	static void writeSettingBool(const QString& setting, const bool value){
		return instance().impl_->writeSettingBool(setting, value);
	}

	static int32_t readSettingInt(const QString& setting,
								  const int32_t defaultRet)
	{
		if( !settingExists( setting)){
			return defaultRet;
		}

		try{
			return readSettingInt( setting );
		} catch (...) {
			return defaultRet;
		}
	}
	static void writeSettingInt(const QString& setting, const int32_t value){
		return instance().impl_->writeSettingInt(setting, value);
	}

	static uint32_t readSettingUInt(const QString& setting,
									const uint32_t defaultRet)
	{
		if( !settingExists( setting)){
			return defaultRet;
		}

		try{
			return readSettingUInt( setting );
		} catch (...) {
			return defaultRet;
		}
	}
	static void writeSettingUInt(const QString& setting,
								 const uint32_t value){
		return instance().impl_->writeSettingUInt(setting, value);
	}


	static QStringList readSettingQStringList(const QString& setting,
											  const QStringList& defaultRet)
	{
		if( !settingExists( setting)){
			return defaultRet;
		}

		try{
			return readSettingQStringList( setting );
		} catch (...) {
			return defaultRet;
		}
	}
	static void writeSettingQStringList(const QString& setting,
										const QStringList& values){
		return instance().impl_->writeSettingQStringList(setting, values);
	}

	static QString readSettingQString(const QString& setting,
									  const QString& defaultRet)
	{
		if( !settingExists( setting)){
			return defaultRet;
		}

		try{
			return readSettingQString( setting );
		} catch (...) {
			return defaultRet;
		}

	}
	static void writeSettingQString(const QString& setting,
									const QString& value){
		return instance().impl_->writeSettingQString(setting, value);
	}

private:
	static bool checkIfSettingExists(const QString& setting){
		return instance().impl_->checkIfSettingExists(setting);
	}

	static int32_t readSettingInt(const QString& setting){
		return instance().impl_->readSettingInt(setting);
	}

	static uint32_t readSettingUInt(const QString& setting){
		return instance().impl_->readSettingUInt(setting);
	}

	static QStringList readSettingQStringList(const QString& setting){
		return instance().impl_->readSettingQStringList(setting);
	}

	static QString readSettingQString(const QString& setting){
		return instance().impl_->readSettingQString(setting);
	}

	static bool readSettingBool(const QString& setting){
		return instance().impl_->readSettingBool(setting);
	}

	LocalPrefFiles()
		: impl_(boost::make_shared<LocalPrefFilesImpl>())
	{}

	boost::shared_ptr<LocalPrefFilesImpl> impl_;

	friend class zi::singleton<LocalPrefFiles>;
};

#endif
