#ifndef OM_CURSORS_H
#define OM_CURSORS_H

#include <QtGui>

class OmBusyCursorWrapper {
 public:
	OmBusyCursorWrapper(){	
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor)); 
	}

	~OmBusyCursorWrapper(){ 
		QApplication::restoreOverrideCursor(); 
	}
};

class OmCursors {
 public:
	static void setToolCursor(QWidget*);
};

#endif
