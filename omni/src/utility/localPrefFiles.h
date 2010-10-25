#ifndef LOCAL_PREF_FILES_H
#define LOCAL_PREF_FILES_H

#include "utility/localPrefFilesImpl.hpp"

class LocalPrefFiles : private om::singletonBase<LocalPrefFiles> {
public:
	inline static bool readSettingBool(const QString& setting,
									   const bool defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingBool(setting);
		} catch (...) {
			return defaultRet;
		}
	}
	inline static void writeSettingBool(const QString& setting,
										const bool value){
		return instance().impl_->writeSettingBool(setting, value);
	}

	inline static int32_t readSettingInt(const QString& setting,
										 const int32_t defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingInt(setting);
		} catch (...) {
			return defaultRet;
		}
	}
	inline static void writeSettingInt(const QString& setting,
									   const int32_t value){
		return instance().impl_->writeSettingInt(setting, value);
	}

	inline static uint32_t readSettingUInt(const QString& setting,
										   const uint32_t defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingUInt(setting);
		} catch (...) {
			return defaultRet;
		}
	}
	inline static void writeSettingUInt(const QString& setting,
										const uint32_t value){
		return instance().impl_->writeSettingUInt(setting, value);
	}

	inline static float readSettingFloat(const QString& setting,
										 const float defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingFloat(setting);
		} catch (...) {
			return defaultRet;
		}
	}
	inline static void writeSettingFloat(const QString& setting,
										 const float value){
		return instance().impl_->writeSettingFloat(setting, value);
	}

	inline static QStringList readSettingQStringList(const QString& setting,
													 const QStringList& defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingQStringList(setting);
		} catch (...) {
			return defaultRet;
		}
	}
	inline static void writeSettingQStringList(const QString& setting,
											   const QStringList& values){
		return instance().impl_->writeSettingQStringList(setting, values);
	}

	inline static QString readSettingQString(const QString& setting,
											 const QString& defaultRet)
	{
		if(!instance().impl_->settingExists(setting)){
			return defaultRet;
		}

		try{
			return instance().impl_->readSettingQString(setting);
		} catch (...) {
			return defaultRet;
		}

	}
	inline static void writeSettingQString(const QString& setting,
										   const QString& value){
		return instance().impl_->writeSettingQString(setting, value);
	}

private:
	LocalPrefFiles()
		: impl_(boost::make_shared<LocalPrefFilesImpl>())
	{}

	boost::shared_ptr<LocalPrefFilesImpl> impl_;

	friend class zi::singleton<LocalPrefFiles>;
};

#endif
