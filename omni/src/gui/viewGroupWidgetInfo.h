#ifndef VIEW_GROUP_WIDGET_INFO_H
#define VIEW_GROUP_WIDGET_INFO_H

#include <QWidget>
#include "common/omCommon.h"

enum WIDGET_TYPE { VIEW2D_CHAN, VIEW2D_SEG, VIEW3D };

class ViewGroupWidgetInfo{
public:
	ViewGroupWidgetInfo(const QString& in_name,
			    const WIDGET_TYPE in_widgetType)
		: name(in_name)
		, widgetType(in_widgetType) {}

	ViewGroupWidgetInfo(const QString& in_name,
			    const WIDGET_TYPE in_widgetType,
			    const ViewType in_vtype)
		: name(in_name)
		, widgetType(in_widgetType)
		, vtype(in_vtype) {}

	QWidget* widget;
	const QString name;
	const WIDGET_TYPE widgetType;

	ViewType vtype;
	Qt::Orientation dir;
};

#endif
