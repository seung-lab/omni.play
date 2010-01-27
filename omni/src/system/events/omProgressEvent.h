#ifndef OM_PROGRESS_EVENT_H
#define OM_PROGRESS_EVENT_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/14/09
 */


#include <common/omStd.h>

#include "system/omEvent.h"
	

class OmProgressEvent : public OmEvent {

public:
	OmProgressEvent(QEvent::Type type);
	OmProgressEvent(QEvent::Type type, int val);	
	OmProgressEvent(QEvent::Type type, string title, int min = 0, int max = 1);	
	OmProgressEvent(QEvent::Type type, int min, int max, int val = 0);
	void Dispatch(OmEventListener *);
	
	//class
	static const OmEventClass CLASS  = OM_PROGRESS_EVENT_CLASS;
	//events
	static const QEvent::Type PROGRESS_SHOW = (QEvent::Type) (CLASS);
	static const QEvent::Type PROGRESS_HIDE = (QEvent::Type) (CLASS+1);
	static const QEvent::Type PROGRESS_RANGE = (QEvent::Type) (CLASS+2);
	static const QEvent::Type PROGRESS_VALUE = (QEvent::Type) (CLASS+3);
	static const QEvent::Type PROGRESS_INCREMENT = (QEvent::Type) (CLASS+4);
	
	string GetText() { return mText; }
	int GetMinimum() { return mMinimum; }
	int GetMaximum() { return mMaximum; }
	int GetValue() { return mValue; }

	static void SetWasCanceled(bool state) { mWasCanceled = state; }
	static bool GetWasCanceled() { return mWasCanceled; }
	
private:
	string mText;
	int mMinimum, mMaximum;
	int mValue;
	static bool mWasCanceled;
};



class OmProgressEventListener : public OmEventListener {
	
public:	
	OmProgressEventListener() : OmEventListener(OmProgressEvent::CLASS) { };

	virtual void ProgressShow(OmProgressEvent *event) { };
	virtual void ProgressHide(OmProgressEvent *event) { };
	
	virtual void ProgressRangeEvent(OmProgressEvent *event) { };
	virtual void ProgressValueEvent(OmProgressEvent *event) { };
	virtual void ProgressIncrementEvent(OmProgressEvent *event) { };
	
};



#endif
