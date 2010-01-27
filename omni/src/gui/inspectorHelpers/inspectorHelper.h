#ifndef INSPECTOR_HELPER_INSPECTOR_HELPER
#define INSPECTOR_HELPER_INSPECTOR_HELPER

#include "../dataWrappers.h"

#include <QWidget>

class MyInspectorWidget;

 template < class T > class InspectorHelper:public QWidget {
 public:
	InspectorHelper(MyInspectorWidget * parent) {
		mParent = parent;
	}

 protected:
	MyInspectorWidget * mParent;
};

#endif
