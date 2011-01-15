#ifndef OM_QT_APP_HPP
#define OM_QT_APP_HPP

#include "system/omLocalPreferences.hpp"

#include <QApplication>

class OmQTApp {
private:
	// from http://www.qtcentre.org/threads/24797-Large-fonts-on-Linux-and-Solaris
	static void setFontSize(const int fontSize)
	{
		QFont appFont = qApp->font();
		appFont.setPointSize(fontSize);
		qApp->setFont(appFont);
	}

public:
	static void SetAppFontSize()
	{
		if(!OmLocalPreferences::FontSizeWasSet()){
			return;
		}

		const int fontSize = OmLocalPreferences::FontSize();
		setFontSize(fontSize);
	}

	static void SetAppFontSize(const int fontSize)
	{
		OmLocalPreferences::FontSize(fontSize);
		setFontSize(fontSize);
	}
};

#endif
