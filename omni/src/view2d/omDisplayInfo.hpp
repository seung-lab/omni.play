#ifndef OM_DISPLAY_INFO_HPP
#define OM_DISPLAY_INFO_HPP

#include "common/omException.h"

#include <QPainter>

class OmDisplayInfo{
public:
	OmDisplayInfo(QPainter& painter, const int initialHeight,
		      const int xoffset)
		: painter_(painter)
		, yTopOfText_(initialHeight)
		, xoffset_(xoffset)
		, yHeightDec_(10)
	{}

	void paint(const QString& str){
		doPaint(str);
	}

	void paint(const int val, const QString& desc){
		doPaint(QString::number(val), desc);
	}

	void paint(const double val, const QString& desc,
		   const int numDecPlaces){
		doPaint(QString::number(val, 'f', numDecPlaces), desc);
	}

	void paint(const float val, const QString& desc,
		   const int numDecPlaces){
		doPaint(QString::number(val, 'f', numDecPlaces), desc);
	}

	void paint(const int val1, const QString& desc1,
		   const int val2, const QString& desc2){
		const QString str =
			QString("%1 %2 %3 %4")
			.arg(val1)
			.arg(desc1)
			.arg(val2)
			.arg(desc2);
		doPaint(str);
	}

private:
	QPainter& painter_;
	int yTopOfText_;
	const int xoffset_;
	const int yHeightDec_;

	void doPaint(const QString& str1, const QString& str2){
		doPaint(str1 + " " + str2);
	}

	void doPaint(const QString& str){
		painter_.drawText(QPoint(xoffset_, yTopOfText_), str);
		yTopOfText_ += yHeightDec_;
	}
};

#endif
